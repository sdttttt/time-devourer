#include <windows.h>
#include "setting_window.h"


static HINSTANCE g_hInstance;

LRESULT CALLBACK settingWndProc(
    _In_ HWND hWnd,
    _In_ UINT message,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
) {
    PAINTSTRUCT ps;
    HDC hdc;

    switch ( message ) 
    {
    case WM_PAINT: {
        hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY: {
        DestroyWindow(hWnd);
        break;
    }

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}


/// <summary>
/// 创建设置窗口类并显示窗口。
/// </summary>
/// <param name="hInstance">应用程序实例的句柄。</param>
/// <param name="nCmdShow">窗口显示方式的标志。</param>
/// <returns>如果成功则返回 0，失败则返回 1。</returns>
int CreateSettingClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow) {
    WNDCLASSEX setting_wcex;

    g_hInstance = hInstance;

    setting_wcex.cbSize = sizeof(WNDCLASSEX);
    setting_wcex.style = CS_HREDRAW | CS_VREDRAW;
    setting_wcex.lpfnWndProc = settingWndProc;
    setting_wcex.cbClsExtra = 0;
    setting_wcex.cbWndExtra = 0;
    setting_wcex.hInstance = hInstance;
    setting_wcex.hIcon = LoadIcon(setting_wcex.hInstance, IDI_APPLICATION);
    setting_wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    setting_wcex.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
    setting_wcex.lpszMenuName = NULL;
    setting_wcex.lpszClassName = SETTING_WINDOW_CLASS;
    setting_wcex.hIconSm = LoadIcon(setting_wcex.hInstance, IDI_APPLICATION);

#ifdef _DEBUG
    auto err = GetLastError();
#endif

    if ( !RegisterClassEx(&setting_wcex) )
    {

        MessageBox(NULL,
            L"RegisterClass failed",
            L"ERROR",
            NULL);


        return 1;
    }

    HWND setting_hwnd = CreateWindowEx(
        0,
        SETTING_WINDOW_CLASS,
        SETTING_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        SETTING_WINDOW_WIDTH, SETTING_WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );


    if ( !setting_hwnd )
    {

        MessageBox(NULL,
            L"Call to CreateWindowEx failed",
            L"ERROR",
            NULL);


        return 1;
    }

    ShowWindow(setting_hwnd, nCmdShow);
    UpdateWindow(setting_hwnd);

    return 0;
}