//
// Created by kms on 2025/9/9.
//

#include "common.h"
#include "task_sched.h"
#include <atlbase.h>

// 错误处理宏: 如果有错误就报告加退出跳转
#define DownIfErr(x, s, ...)        \
    if (FAILED(x))                  \
    {                               \
        Debug::ReportError(s);      \
        goto DOWN;                  \
    }

// 错误处理宏: 报告错误加退出跳转
#define DownWithErr(s)     \
    Debug::ReportError(s); \
    goto DOWN;

// 退出宏
#define DownOk() \
    goto DOWN;


void CreateStartupTask()
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

    COMPtr<ITaskService> p_service(nullptr);
    COMPtr<ITaskFolder> p_folder(nullptr);
    COMPtr<ITaskFolder> p_root_folder(nullptr);
    COMPtr<IRegisteredTask> p_reg_task(nullptr);
    COMPtr<IRegistrationInfo> p_reg_info(nullptr);
    COMPtr<ITaskDefinition> p_task_def(nullptr);
    COMPtr<ITaskSettings> p_task_settings(nullptr);
    COMPtr<ITriggerCollection> p_trigger_collect(nullptr);

    if (0 == GetEnvironmentVariable(ENV_USERNAME, username.data(), USERNAME_LEN))
    {
        DownWithErr(L"环境变量:USERNAME获取失败");
    }
    if (0 == GetEnvironmentVariable(ENV_USERDOMAIN, username_domain.data(), USERNAME_DOMAIN_LEN))
    {
        DownWithErr(L"环境变量:USERDOMAIN获取失败");
    }

    username_domain += L"\\";
    username_domain += username;

    task_name += L"timer-";
    task_name += username;

    hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    DownIfErr(hr, L"COM组件:初始化错误");

    hr = CoCreateInstance(
        __uuidof(TaskScheduler),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(ITaskService),
        reinterpret_cast<void**>(p_service.AsOutPtr())
    );
    DownIfErr(hr, L"COM组件TaskScheduler:创建失败");

    hr = p_service->Connect(
        _variant_t(),
        _variant_t(),
        _variant_t(),
        _variant_t());
    DownIfErr(hr, L"TaskScheduler:连接失败");

    // 尝试获取任务目录
    hr = p_service->GetFolder(_bstr_t(TASK_SCHED_FOLDER), p_folder.AsOutPtr());

    // 如果失败，断言为目录不存在，尝试创建
    if (FAILED(hr))
    {
        hr = p_service->GetFolder(
            _bstr_t(TASK_SCHED_ROOT),
            p_root_folder.AsOutPtr());
        DownIfErr(hr, L"TaskScheduler:获取根目录失败");

        hr = p_root_folder->CreateFolder(
            _bstr_t(TASK_SCHED_FOLDER),
            _variant_t(),
            p_folder.AsOutPtr());
        DownIfErr(hr, L"TaskScheduler:根目录创建目录失败");
    }

    // 尝试启动任务
    hr = p_folder->GetTask(_bstr_t(task_name.c_str()), p_reg_task.AsOutPtr());
    if (SUCCEEDED(hr))
    {
        hr = p_reg_task->put_Enabled(VARIANT_TRUE);
        if (SUCCEEDED(hr))
        {
            DownOk();
        }
    }



    hr = p_service->NewTask(0, p_task_def.AsOutPtr());
    DownIfErr(hr, L"创建任务错误");

    hr = p_task_def->get_RegistrationInfo(p_reg_info.AsOutPtr());
    DownIfErr(hr, L"创建任务:注册信息获取失败")

    hr = p_reg_info->put_Author(_bstr_t(username_domain.c_str()));
    DownIfErr(hr, L"创建任务:put_Author发生错误")

    hr = p_task_def->get_Settings(p_task_settings.AsOutPtr());
    DownIfErr(hr, L"创建任务:get_Settings发生错误");

    // 获取或设置一个布尔值，该值指示任务计划程序可以在其计划时间过后随时启动任务。
    hr = p_task_settings->put_StartWhenAvailable(VARIANT_FALSE);
    DownIfErr(hr, L"任务设置:put_StartWhenAvailable发生错误");

    // 获取或设置一个布尔值，该值指示在计算机使用电池时任务将停止。
    hr = p_task_settings->put_StopIfGoingOnBatteries(VARIANT_FALSE);
    DownIfErr(hr, L"任务设置:put_StopIfGoingOnBatteries发生错误");

    // 获取或设置完成任务所允许的时间量。 默认情况下，任务将在开始运行 72 小时后停止。 可以通过更改此设置来更改此设置。
    hr = p_task_settings->put_ExecutionTimeLimit(_bstr_t(L"PT0S")); //Unlimited
    DownIfErr(hr, L"任务设置:put_ExecutionTimeLimit发生错误");

    // 获取或设置一个布尔值，该值指示如果计算机使用电池运行，则不会启动任务
    hr = p_task_settings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
    DownIfErr(hr, L"任务设置:put_DisallowStartIfOnBatteries发生错误");


    hr = p_task_def->get_Triggers(p_trigger_collect.AsOutPtr());
    DownIfErr(hr, L"任务触发器Collection获取失败");


    COMPtr<ITrigger> p_trigger(nullptr);
    COMPtr<ILogonTrigger> p_toggle(nullptr);
    // 用户登陆时触发
    hr = p_trigger_collect->Create(TASK_TRIGGER_LOGON, p_trigger.AsOutPtr());
    DownIfErr(hr, L"触发器创建失败");

    hr = p_trigger->QueryInterface(__uuidof(ILogonTrigger), p_toggle.AsOutVoidPtr());
    DownIfErr(hr, L"QueryInterface触发器获取失败")

    hr = p_toggle->put_Id(_bstr_t(L"T1"));
    DownIfErr(hr, L"ID设置失败");

    hr = p_toggle->put_Delay(_bstr_t(DELAY_STARTUP));
    DownIfErr(hr, L"延时设置失败");

    hr = p_toggle->put_UserId(_bstr_t(username_domain.c_str()));
    DownIfErr(hr, L"设置触发器用户ID错误");


    COMPtr<IActionCollection> p_action_collect(nullptr);
    COMPtr<IAction> p_act(nullptr);
    COMPtr<IExecAction> p_exec_act(nullptr);

    // Get the task action collection pointer.
    hr = p_task_def->get_Actions(p_action_collect.AsOutPtr());
    DownIfErr(hr, L"无法创建IActionCollection");

    // Create the action, specifying that it is an executable action.
    hr = p_action_collect->Create(TASK_ACTION_EXEC, p_act.AsOutPtr());

    DownIfErr(hr, L"无法创建Action");
    // QI for the executable task pointer.
    hr = p_act->QueryInterface(
        __uuidof(IExecAction), p_exec_act.AsOutVoidPtr());
    DownIfErr(hr, L"获取IExecAction失败");

    // Set the path of the executable to TrafficMonitor (passed as CustomActionData).
    hr = p_exec_act->put_Path(_bstr_t(self_exec_path.c_str()));
    DownIfErr(hr, L"设置执行路径失败");


    COMPtr<IPrincipal> p_principal(nullptr);
    hr = p_task_def->get_Principal(p_principal.AsOutPtr());
    DownIfErr(hr, L"获取Principal失败");

    hr = p_principal->put_Id(_bstr_t(L"P1"));
    DownIfErr(hr, L"Principal ID失败");
    hr = p_principal->put_UserId(_bstr_t(username_domain.c_str()));
    DownIfErr(hr, L"Principal put_UserId失败");
    hr = p_principal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
    DownIfErr(hr, L"获取Principal LogonType失败");


DOWN:

    CoUninitialize();
}
