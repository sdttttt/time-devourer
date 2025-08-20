#pragma once

#include <windows.h>


namespace Tray {
	constexpr UINT ICON_ID = 2;
	constexpr UINT WM_ICON = WM_USER + 1;

	void AddIcon(HWND h_wnd, HICON h_icon);
	void RemoveIcon();
	void ShowMenu(HWND hWnd);
};