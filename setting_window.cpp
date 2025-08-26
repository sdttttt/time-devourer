#include <windows.h>
#include "setting_window.h"
#include "resource.h"
#include "common.h"


static LRESULT CALLBACK settingWndProc(
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

    case WM_CLOSE: {
        DestroyWindow(hWnd);
        break;
    }

    case WM_DESTROY: {
        break;
    }

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}


static void registerSettingClass(_In_ HINSTANCE hInstance) {
    WNDCLASSEX setting_wcex;

    setting_wcex.cbSize = sizeof(WNDCLASSEX);
    setting_wcex.style = CS_HREDRAW | CS_VREDRAW;
    setting_wcex.lpfnWndProc = settingWndProc;
    setting_wcex.cbClsExtra = 0;
    setting_wcex.cbWndExtra = 0;
    setting_wcex.hInstance = hInstance;
    setting_wcex.hIcon = LoadIcon(setting_wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));
    setting_wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    setting_wcex.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
    setting_wcex.lpszMenuName = NULL;
    setting_wcex.lpszClassName = TEST_CLASS_NAME;
    setting_wcex.hIconSm = LoadIcon(setting_wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));


    RegisterClassEx(&setting_wcex);
    Debug::TryLastErrorMessageBox();
}

/// <summary>
/// 创建设置窗口类并显示窗口。
/// </summary>
/// <param name="hInstance">应用程序实例的句柄。</param>
/// <param name="nCmdShow">窗口显示方式的标志。</param>
/// <returns>如果成功则返回 0，失败则返回 1。</returns>
int CreateSettingClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow) {

    WNDCLASSEX setting_wcex = { sizeof(WNDCLASSEX) };
    if ( FALSE == GetClassInfoEx(hInstance, TEST_CLASS_NAME, &setting_wcex) ) {
        registerSettingClass(hInstance);
    }

    HWND setting_hwnd = CreateWindowEx(
        0,
        TEST_CLASS_NAME,
        TEST_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        TEST_WINDOW_WIDTH, TEST_WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    Debug::TryLastErrorMessageBox();

    ShowWindow(setting_hwnd, nCmdShow);
    UpdateWindow(setting_hwnd);

    return 0;
}
