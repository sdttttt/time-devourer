#include <windows.h>


#include "task_sched.h"
#include "resource.h"
#include "tray.h"

namespace Tray
{
    NOTIFYICONDATA nid;

    void AddIcon(HWND h_wnd)
    {
        HICON h_icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));

        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = h_wnd;
        nid.uID = ICON_ID;
        nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
        nid.uCallbackMessage = WM_ICON;
        nid.hIcon = h_icon;

        Shell_NotifyIcon(NIM_ADD, &nid);
    }

    void RemoveIcon()
    {
        Shell_NotifyIcon(NIM_DELETE, &nid);
    }

    void ShowMenu(HWND hWnd)
    {
        POINT pt;
        GetCursorPos(&pt);

        // 创建弹出菜单
        HMENU h_menu = CreatePopupMenu();

        std::wstring current_exec_path;
        auto is_active_startup = TaskSched::IsActiveStartupTask(&current_exec_path);

#ifdef  _DEBUG
        AppendMenu(h_menu, MF_STRING, TRAY_MENU_TEST, L"调试");
#endif
        // TODO: 暂时禁用设置菜单，没啥好用的
        // AppendMenu(h_menu, MF_GRAYED, TRAY_MENU_SETTING, L"Setting");
        AppendMenu(h_menu, is_active_startup ? MF_CHECKED : MF_UNCHECKED, TRAY_MENU_STARTUP, L"开机启动");
        AppendMenu(h_menu, MF_STRING, TRAY_MENU_EXIT, L"退出");

        // 激活前台窗口
        SetForegroundWindow(hWnd);

        TrackPopupMenu(
            h_menu,
            TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
            pt.x, pt.y,
            0,
            hWnd,
            NULL);

        DestroyMenu(h_menu);
    }
};
