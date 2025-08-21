#include <windows.h>
#include "tray.h"
#include "date.h"
#include "font.h"
#include "resource.h"

// 时间的更新定时器
constexpr UINT_PTR DATE_TIMER_ID = 0x0001;
// 动画的更新定时器
constexpr UINT_PTR ANIMATION_TIMER_ID = 0x0002;
// 名称
constexpr TCHAR SZ_WINDOW_CLASS[] = L"TIMER";
// 标题栏名
constexpr TCHAR SZ_TITLE[] = L"TIMER";

// 窗口尺寸
constexpr UINT WINDOW_WIDTH = 400;
constexpr UINT WINDOW_HEIGHT = 100;

// 窗口显示时间半径: 秒
constexpr UINT WINDOWS_SHOW_TIME_RADIUS_SEC = 30;

void Exit(HWND hwnd) {
	KillTimer(hwnd, DATE_TIMER_ID);
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

	// 窗口移动状态
	static BOOL is_dragging = FALSE;
	// 鼠标拖动位置起点
	static POINT drag_start;

	// 透明度
	static int g_alpha = 255;
	// 窗口动画标识
	static BOOL g_fading = TRUE;
	// 动画速度
	constexpr int FADE_DURATION = 5;

	switch (message)
	{
	case WM_CREATE: {
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
		if (wParam == DATE_TIMER_ID)
		{	
			InvalidateRect(hWnd, NULL, TRUE);
			
		}

		if (wParam == ANIMATION_TIMER_ID)
		{
			if (g_fading) {
				g_alpha += FADE_DURATION;
				if (g_alpha >= 255) {
					g_alpha = 255;
					g_fading = FALSE;
					KillTimer(hWnd, ANIMATION_TIMER_ID);
				}
			}
			else {
				g_alpha -= FADE_DURATION;
				if (g_alpha <= 0) {
					g_alpha = 0;
					g_fading = TRUE;
					KillTimer(hWnd, ANIMATION_TIMER_ID);
				}
			}

			SetLayeredWindowAttributes(hWnd, 0, g_alpha, LWA_ALPHA);
		}
		break;
	}

	// 鼠标按下
	case WM_LBUTTONDOWN: {
		// 鼠标位置
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };

		RECT rect;
		// 获得窗口矩形
		GetClientRect(hWnd, &rect);
		RECT area_rect = rect;
		
		// 检查鼠标位置是否在窗口矩形内
		if (PtInRect(&area_rect, pt)) {
			drag_start = pt;
			is_dragging = TRUE;

			// 设置鼠标捕获以及修改鼠标样式
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
		auto time_str = Date::CurrTimeWStr();

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


	HWND hWnd = CreateWindowEx(
		0,
		SZ_WINDOW_CLASS,
		SZ_TITLE,
		WS_POPUP | WSF_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	// 设置启动时的不透明度
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

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

	SetTimer(hWnd, DATE_TIMER_ID, 1000, NULL);

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

