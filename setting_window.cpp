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
    setting_wcex.lpszClassName = SETTING_CLASS_NAME;
    setting_wcex.hIconSm = LoadIcon(setting_wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));


    RegisterClassEx(&setting_wcex);
    Debug::TryLastErrorMessageBox();
}

/// <summary>
/// �������ô����ಢ��ʾ���ڡ�
/// </summary>
/// <param name="hInstance">Ӧ�ó���ʵ���ľ����</param>
/// <param name="nCmdShow">������ʾ��ʽ�ı�־��</param>
/// <returns>����ɹ��򷵻� 0��ʧ���򷵻� 1��</returns>
int CreateSettingClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow) {

    WNDCLASSEX setting_wcex = { sizeof(WNDCLASSEX) };
    if ( FALSE == GetClassInfoEx(hInstance, SETTING_CLASS_NAME, &setting_wcex) ) {
        registerSettingClass(hInstance);
    }

    HWND setting_hwnd = CreateWindowEx(
        0,
        SETTING_CLASS_NAME,
        SETTING_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        SETTING_WINDOW_WIDTH, SETTING_WINDOW_HEIGHT,
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