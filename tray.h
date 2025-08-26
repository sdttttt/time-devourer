#pragma once

#include <windows.h>

namespace Tray {
    constexpr auto ICON_ID = 2;
    constexpr auto WM_ICON = WM_USER + 1;
    constexpr auto TRAY_MENU_EXIT = 1001;
    constexpr auto TRAY_MENU_SETTING = 1002;

    constexpr auto TRAY_MENU_TEST = 1080;

    void AddIcon(HWND h_wnd);
    void RemoveIcon();
    void ShowMenu(HWND hWnd);
};
