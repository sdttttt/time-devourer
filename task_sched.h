//
// Created by kms on 2025/9/9.
//
#pragma once

#include <Lmcons.h>


// 计划任务的任务路径
constexpr auto TASK_SCHED_ROOT = L"\\";
constexpr auto TASK_SCHED_FOLDER = L"\\TimeDevourer";

constexpr auto ENV_USERNAME = L"USERNAME";
constexpr auto ENV_USERDOMAIN = L"USERDOMAIN";
constexpr auto DELAY_STARTUP = L"PT05S"; // 延时5秒启动

constexpr auto SDDL_FULL_ACCESS_FOR_EVERYONE = L"D:(A;;FA;;;WD)";


constexpr auto USERNAME_DOMAIN_LEN = DNLEN + 2; // Domain Name + '\' + User Name + '\0'
constexpr auto USERNAME_LEN = UNLEN + 1; // User Name + '\0'

namespace TaskSched
{
    /**
     * 创建开机计划任务
     * @return
     */
    BOOL CreateStartupTask();
    /**
     * 删除开机计划任务
     * @return
     */
    BOOL DeleteStartupTask();
    /**
     * 检查任务是否存在
     * @param path
     * @return
     */
    BOOL IsActiveStartupTask(std::wstring* path);
};


/**
 * //--------------------- 后面发现官方有自动包装的CComPtr，努力全部木大
 *
 * 自动包装COM对象指针，生命周期结束自动调用Release
 * 要求T必须能调用Release方法，且T必须是IUnknown的子类
 *
 * 头一次写模版元编程给我肘晕了，这就是编译期编程么，害怕
 * 这里稍微总结一下几个点,最上面的is_com_interface有两个模版参数,
 * 第一个是T, 第二个是void, void没什么意义，主要用来做特化匹配.
 *
 * 下面是重点，typename T只有一个参数，默认外部调用的都是这个模板，例如COMPtr<ITaskService>
 * 这个模板被调用的时候，在编译器会根据下面的第二个模版参数做类型检查：
 *
 * std::void_t<条件...> 这个条件如果成立，就会变成void, 就能用上这个模版了
 * 否则就会失败匹配到上面的typename = void的默认模板
 *
 * 第一个条件：decltype(std::declval<T>().Release())
 * std::declvar<Type> 这个函数返回一个Type的对象, 用来模拟运行时的情况，然后可以模拟调用运行时的方法.
 * decltype是一个类型提取器，返回值就是一个Type，运行方式sizeof很像，只能在编译期运行，不能在运行时调用。
 * 如果declval模拟的对象的Release方法调用失败了，那就没有返回值，decltype就会报错，就无法匹配到这个模版了
 *
 * 第二个条件：std::enable_if_t<std::is_base_of_v<IUnknown, T>>
 * 这个简单一些，判断T是否是IUnknown的子类，是就enable_if_t<true> 返回 void
 * 不是就enable_if_t<false> 触发SFINAE,退回上面的模版
 */
template <typename T, typename = void>
struct is_com_interface : std::false_type
{
};

template <typename T>
struct is_com_interface<
        T,
        std::void_t<
            decltype(std::declval<T>().Release()),
            std::enable_if_t<std::is_base_of_v<IUnknown, T>>
        >
    > : std::true_type
{
};

template <typename T>
class COMPtr
{
    static_assert(is_com_interface<T>::value, "Type is not a COM object");
    T* com_ptr;

public:
    COMPtr() : com_ptr(nullptr)
    {
    }

    COMPtr(std::nullptr_t) : com_ptr(nullptr)
    {
    }

    COMPtr(T* p) : com_ptr(p)
    {
    }

    ~COMPtr()
    {
        if (com_ptr) com_ptr->Release();
    };

    explicit operator bool() const { return com_ptr != nullptr; }

    T* Get() { return com_ptr; }
    T* operator->() { return com_ptr; }

    T** AsOutPtr()
    {
        if (com_ptr)
        {
            com_ptr->Release();
        }
        com_ptr = nullptr;
        return &com_ptr;
    }

    void** AsOutVoidPtr()
    {
        if (com_ptr)
        {
            com_ptr->Release();
        }
        com_ptr = nullptr;
        return reinterpret_cast<void**>(&com_ptr);
    }
};

/**
 * 自动释放COM
 */
class COMScope
{
    BOOL b_init = FALSE;
public:
    COMScope() = default;
    ~COMScope()
    {
        if (b_init)
        {
            CoUninitialize();
        }
    }

    HRESULT init()
    {
        if (b_init)
        {
            return S_OK;
        }

        auto hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (SUCCEEDED(hr))
        {
            b_init = TRUE;
        }
        return hr;
    }
};
