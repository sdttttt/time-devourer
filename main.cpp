#include <windows.h>
#include "tray.h"
#include "date.h"
#include "font.h"
#include "resource.h"
#include "proc.h"

// 名称
constexpr TCHAR SZ_WINDOW_CLASS[] = L"TIMER";
// 标题栏名
constexpr TCHAR SZ_TITLE[] = L"TIMER";

// 窗口尺寸
constexpr UINT WINDOW_WIDTH = 400;
constexpr UINT WINDOW_HEIGHT = 100;


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
		WS_EX_LAYERED,
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
	SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);

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
			L"ERROR",
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

