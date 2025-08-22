#pragma once

#include <windows.h>


namespace Tray {
    constexpr UINT ICON_ID = 2;
    constexpr UINT WM_ICON = WM_USER + 1;
    constexpr UINT TRAY_MENU_EXIT = 1001;
    constexpr UINT TRAY_MENU_SETTING = 1002;

    void AddIcon(HWND h_wnd, HICON h_icon);
    void RemoveIcon();
    void ShowMenu(HWND hWnd);
};