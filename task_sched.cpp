//
// Created by kms on 2025/9/9.
//

#include "common.h"
#include "task_sched.h"
#include <atlbase.h>

template<typename F>
BOOL TryInvoke(F&& func, LPCWSTR msg)
{
    if (auto hr = func(); FAILED(hr))
    {
        Debug::ReportError(msg);
        return hr;
    }

    return S_OK;
}

// 尝试执行一个步骤
#define TryStep(func, msg) \
    do { auto r = TryInvoke([&] { return func; }, msg); if (r) { return FALSE; } } while (0);

#define ErrThrow(msg) \
    do { TryInvoke([&] { return E_FAIL; }, msg); return FALSE; } while (0);

/**
 *
 * @return BOOL
 */
BOOL CreateStartupTask()
{
    // 错误位标志
    auto hr = S_OK;

    std::wstring task_name;

    std::wstring username_domain;
    username_domain.resize(USERNAME_DOMAIN_LEN);

    std::wstring username;
    username.resize(USERNAME_LEN);

    // 自己的执行路径
    std::wstring self_exec_path;
    self_exec_path.resize(MAX_PATH);
    GetModuleFileName(NULL, self_exec_path.data(), MAX_PATH);

    COMScope com_scope;
    COMPtr<ITaskService> p_service(nullptr);
    COMPtr<ITaskFolder> p_folder(nullptr);
    COMPtr<ITaskFolder> p_root_folder(nullptr);
    COMPtr<IRegisteredTask> p_reg_task(nullptr);
    COMPtr<IRegistrationInfo> p_reg_info(nullptr);
    COMPtr<ITaskDefinition> p_task_def(nullptr);
    COMPtr<ITaskSettings> p_task_settings(nullptr);
    COMPtr<ITriggerCollection> p_trigger_collect(nullptr);

    const auto username_len = GetEnvironmentVariable(ENV_USERNAME, username.data(), USERNAME_LEN);
    const auto userdomain_len = GetEnvironmentVariable(ENV_USERDOMAIN, username_domain.data(), USERNAME_DOMAIN_LEN);
    if (0 == username_len)
    {
        ErrThrow(L"环境变量:USERNAME获取失败");
    }
    if (0 == userdomain_len)
    {
        ErrThrow(L"环境变量:USERDOMAIN获取失败");
    }

    username_domain += L"\\";
    username_domain += username;

    task_name += L"timer-";
    task_name += username;

    TryStep(com_scope.init(), L"COM组件:初始化错误");


    TryStep(CoCreateInstance(
        __uuidof(TaskScheduler),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(ITaskService),
        reinterpret_cast<void**>(p_service.AsOutPtr()))
        , L"COM组件TaskScheduler:创建失败");

    TryStep(p_service->Connect(
        _variant_t(),
        _variant_t(),
        _variant_t(),
        _variant_t())
        , L"TaskScheduler:连接失败");

    // 尝试获取任务目录
    hr = p_service->GetFolder(_bstr_t(TASK_SCHED_FOLDER), p_folder.AsOutPtr());

    // 如果失败，断言为目录不存在，尝试创建
    if (FAILED(hr))
    {
        hr = p_service->GetFolder(
            _bstr_t(TASK_SCHED_ROOT),
            p_root_folder.AsOutPtr());
        TryStep(hr, L"TaskScheduler:获取根目录失败");

        hr = p_root_folder->CreateFolder(
            _bstr_t(TASK_SCHED_FOLDER),
            _variant_t(),
            p_folder.AsOutPtr());
        TryStep(hr, L"TaskScheduler:根目录创建目录失败");
    }

    // 尝试启动任务
    hr = p_folder->GetTask(_bstr_t(task_name.c_str()), p_reg_task.AsOutPtr());
    if (SUCCEEDED(hr))
    {
        hr = p_reg_task->put_Enabled(VARIANT_TRUE);
        if (SUCCEEDED(hr))
        {
            return TRUE;
        }
    }

    hr = p_service->NewTask(0, p_task_def.AsOutPtr());
    TryStep(hr, L"创建任务错误");

    hr = p_task_def->get_RegistrationInfo(p_reg_info.AsOutPtr());
    TryStep(hr, L"创建任务:注册信息获取失败")

    hr = p_reg_info->put_Author(_bstr_t(username_domain.c_str()));
    TryStep(hr, L"创建任务:put_Author发生错误")

    hr = p_task_def->get_Settings(p_task_settings.AsOutPtr());
    TryStep(hr, L"创建任务:get_Settings发生错误");

    // 获取或设置一个布尔值，该值指示任务计划程序可以在其计划时间过后随时启动任务。
    hr = p_task_settings->put_StartWhenAvailable(VARIANT_FALSE);
    TryStep(hr, L"任务设置:put_StartWhenAvailable发生错误");

    // 获取或设置一个布尔值，该值指示在计算机使用电池时任务将停止。
    hr = p_task_settings->put_StopIfGoingOnBatteries(VARIANT_FALSE);
    TryStep(hr, L"任务设置:put_StopIfGoingOnBatteries发生错误");

    // 获取或设置完成任务所允许的时间量。 默认情况下，任务将在开始运行 72 小时后停止。 可以通过更改此设置来更改此设置。
    hr = p_task_settings->put_ExecutionTimeLimit(_bstr_t(L"PT05S")); //Unlimited
    TryStep(hr, L"任务设置:put_ExecutionTimeLimit发生错误");

    // 获取或设置一个布尔值，该值指示如果计算机使用电池运行，则不会启动任务
    hr = p_task_settings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
    TryStep(hr, L"任务设置:put_DisallowStartIfOnBatteries发生错误");


    hr = p_task_def->get_Triggers(p_trigger_collect.AsOutPtr());
    TryStep(hr, L"任务触发器Collection获取失败");


    COMPtr<ITrigger> p_trigger(nullptr);
    COMPtr<ILogonTrigger> p_toggle(nullptr);
    // 用户登陆时触发
    hr = p_trigger_collect->Create(TASK_TRIGGER_LOGON, p_trigger.AsOutPtr());
    TryStep(hr, L"触发器创建失败");

    hr = p_trigger->QueryInterface(__uuidof(ILogonTrigger), p_toggle.AsOutVoidPtr());
    TryStep(hr, L"QueryInterface触发器获取失败")

    hr = p_toggle->put_Id(_bstr_t(L"T1"));
    TryStep(hr, L"ID设置失败");

    hr = p_toggle->put_Delay(_bstr_t(DELAY_STARTUP));
    TryStep(hr, L"延时设置失败");

    hr = p_toggle->put_UserId(_bstr_t(username_domain.c_str()));
    TryStep(hr, L"设置触发器用户ID错误");


    COMPtr<IActionCollection> p_action_collect(nullptr);
    COMPtr<IAction> p_act(nullptr);
    COMPtr<IExecAction> p_exec_act(nullptr);

    // Get the task action collection pointer.
    hr = p_task_def->get_Actions(p_action_collect.AsOutPtr());
    TryStep(hr, L"无法创建IActionCollection");

    // Create the action, specifying that it is an executable action.
    hr = p_action_collect->Create(TASK_ACTION_EXEC, p_act.AsOutPtr());

    TryStep(hr, L"无法创建Action");
    // QI for the executable task pointer.
    hr = p_act->QueryInterface(
        __uuidof(IExecAction), p_exec_act.AsOutVoidPtr());
    TryStep(hr, L"获取IExecAction失败");

    // Set the path of the executable to TrafficMonitor (passed as CustomActionData).
    hr = p_exec_act->put_Path(_bstr_t(self_exec_path.c_str()));
    TryStep(hr, L"设置执行路径失败");


    COMPtr<IPrincipal> p_principal(nullptr);
    hr = p_task_def->get_Principal(p_principal.AsOutPtr());
    TryStep(hr, L"获取Principal失败");

    hr = p_principal->put_Id(_bstr_t(L"P1"));
    TryStep(hr, L"Principal ID失败");
    // 指定帐户时，请记得在代码中正确使用双反斜杠来指定域和用户名。 例如，使用 DOMAIN\UserName 为 UserId 属性指定值。
    hr = p_principal->put_UserId(_bstr_t(username_domain.c_str()));
    TryStep(hr, L"Principal put_UserId失败");
    hr = p_principal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
    TryStep(hr, L"获取Principal LogonType失败");

    // 最低权限运行
    hr = p_principal->put_RunLevel(_TASK_RUNLEVEL::TASK_RUNLEVEL_LUA);
    TryStep(hr, L"put_RunLevel失败");


    hr = p_folder->RegisterTaskDefinition(
        _bstr_t(task_name.c_str()),
        p_task_def.Get(),
        TASK_CREATE_OR_UPDATE,
        _variant_t(username_domain.c_str()),
        _variant_t(),
        TASK_LOGON_INTERACTIVE_TOKEN,
        _variant_t(),
        p_reg_task.AsOutPtr()
    );
    TryStep(hr, L"RegisterTaskDefinition失败");

    return SUCCEEDED(hr);
}
