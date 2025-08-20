#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <sstream>
#include "tray.h"
#include "timer.h"
#include "font.h"
#include "resource.h"


constexpr UINT TIMER_ID = 1;
constexpr TCHAR SZ_WINDOW_CLASS[] = L"TIMER";
constexpr TCHAR SZ_TITLE[] = L"TIMER";

constexpr UINT WINDOW_WIDTH = 400;
constexpr UINT WINDOW_HEIGHT = 100;

HINSTANCE hInst;

// 移除标题栏
void RemoveTitleBar(HWND hWnd) {
	// 移除标题栏相关样式
	LONG style = GetWindowLong(hWnd, GWL_STYLE);
	style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	SetWindowLong(hWnd, GWL_STYLE, style);

	// 移除扩展边框样式
	LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	SetWindowLong(hWnd, GWL_EXSTYLE, exStyle);

	// 重绘窗口边框
	SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
		SWP_FRAMECHANGED | SWP_NOACTIVATE);
}

BOOL IsPointInArea(HWND hwnd, POINT pt) {
	RECT rect;
	GetClientRect(hwnd, &rect);

	RECT area_rect = rect;

	return PtInRect(&area_rect, pt);
}


void Exit(HWND hwnd) {
	KillTimer(hwnd, TIMER_ID);
	PostQuitMessage(0);
}

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	PAINTSTRUCT ps;
	HDC hdc;

	static BOOL is_dragging = FALSE;
	static POINT drag_start;

	switch (message)
	{
	case WM_CREATE: {
		SetTimer(hWnd, TIMER_ID, 1000, NULL);
		HICON h_icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		Tray::AddIcon(hWnd, h_icon);
		break;
	}

	case Tray::WM_ICON: {
		if (lParam == WM_RBUTTONUP) {
			Tray::ShowMenu(hWnd);
		}

		if (lParam == WM_LBUTTONDBLCLK) {
			ShowWindow(hWnd, SW_SHOW);
			SetForegroundWindow(hWnd);
		}
	}

	case WM_TIMER: {
		if (wParam == TIMER_ID) InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	case WM_LBUTTONDOWN: {
		// 鼠标位置
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };

		if (IsPointInArea(hWnd, pt)) {
			drag_start = pt;
			is_dragging = TRUE;

			SetCapture(hWnd);
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		}

		break;
	}

	case WM_LBUTTONUP: {
		if (is_dragging) {
			is_dragging = FALSE;
			ReleaseCapture();
		}

		break;
	}

	case WM_MOUSEMOVE: {
		if (is_dragging) {
			POINT pt;
			GetCursorPos(&pt);

			RECT rect;
			GetWindowRect(hWnd, &rect);

			int new_x = rect.left + (pt.x - (rect.left + drag_start.x));
			int new_y = rect.top + (pt.y - (rect.top + drag_start.y));

			// 移动窗口
			SetWindowPos(hWnd, NULL, new_x, new_y, 0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
			case Tray::TRAY_MENU_SETTING: {
				break;
			}

			case Tray::TRAY_MENU_EXIT: {
				Exit(hWnd);
				break;
			}
		}
	}

	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
		auto time_str = Date::Curr();

		RECT rect;
		GetClientRect(hWnd, &rect);
		Font::DrawScaledText(hdc, rect, time_str);

		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY:
		Exit(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
};



int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = SZ_WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);


	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			L"Call to RegisterClassEx failed!",
			L"Windows Desktop Guided Tour",
			NULL);

		return 1;
	}

	hInst = hInstance;

	HWND hWnd = CreateWindowEx(
		WS_OVERLAPPED,
		SZ_WINDOW_CLASS,
		SZ_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	// 删除窗口的标题栏
	RemoveTitleBar(hWnd);

	// 圆角
	HRGN h_rgn = CreateRoundRectRgn(
		0, 0, 
		WINDOW_WIDTH, WINDOW_HEIGHT,
		10, 10);

	SetWindowRgn(hWnd, h_rgn, TRUE);

	SetWindowPos(
		hWnd, NULL, 
		10, 10, 
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	if (!hWnd)
	{
		MessageBox(NULL,
			L"Call to CreateWindowEx failed!",
			L"Windows Desktop Guided Tour",
			NULL);

		return 1;
	}


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
};

