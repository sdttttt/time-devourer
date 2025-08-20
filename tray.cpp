#include <windows.h>

#include "tray.h"

namespace Tray {

	constexpr UINT TRAY_MENU_EXIT = 1001;
	constexpr UINT TRAY_MENU_SETTING = 1002;

	NOTIFYICONDATA nid;

	void AddIcon(HWND h_wnd, HICON h_icon) {
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = h_wnd;
		nid.uID = ICON_ID;
		nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		nid.uCallbackMessage = WM_ICON;
		nid.hIcon = h_icon;
		
		wcscpy_s(nid.szTip, L"TIMER");

		Shell_NotifyIcon(NIM_ADD, &nid);
	}

	void RemoveIcon() {
		Shell_NotifyIcon(NIM_DELETE, &nid);
	}

	void ShowMenu(HWND hWnd) {
		POINT pt;
		GetCursorPos(&pt);

		// 创建弹出菜单
		HMENU h_menu = CreatePopupMenu();

		AppendMenu(h_menu, MF_STRING, TRAY_MENU_SETTING, L"Setting");
		AppendMenu(h_menu, MF_STRING, TRAY_MENU_EXIT, L"Exit");

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

