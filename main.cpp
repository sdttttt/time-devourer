#include <windows.h>
#include "time_proc.h"

// ʱ����ʾ������
constexpr TCHAR TIMER_WINDOW_CLASS[ ] = L"TimeWindowClass";
// ���ô�����
constexpr TCHAR SETTING_WINDOW_CLASS[ ] = L"SettingWindowClass";

// ��������
constexpr TCHAR SZ_TITLE[ ] = L"TIMER";

// ���ڳߴ�
constexpr UINT WINDOW_WIDTH = 400;
constexpr UINT WINDOW_HEIGHT = 100;


/// <summary>
/// ����һ����ʱ�������ಢ��ʾ���ڣ����ô�����ʽ��Բ�ǺͶ�ʱ����
/// </summary>
/// <param name="hInstance">Ӧ�ó���ʵ�������</param>
/// <param name="nCmdShow">������ʾ��ʽ��</param>
/// <returns>����ɹ��򷵻�0�����ʧ���򷵻�1��</returns>
int CreateTimeClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow) {
    WNDCLASSEX time_wcex;

    time_wcex.cbSize = sizeof(WNDCLASSEX);
    time_wcex.style = CS_HREDRAW | CS_VREDRAW;
    time_wcex.lpfnWndProc = WndProc;
    time_wcex.cbClsExtra = 0;
    time_wcex.cbWndExtra = 0;
    time_wcex.hInstance = hInstance;
    time_wcex.hIcon = LoadIcon(time_wcex.hInstance, IDI_APPLICATION);
    time_wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    time_wcex.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
    time_wcex.lpszMenuName = NULL;
    time_wcex.lpszClassName = TIMER_WINDOW_CLASS;
    time_wcex.hIconSm = LoadIcon(time_wcex.hInstance, IDI_APPLICATION);


    if ( !RegisterClassEx(&time_wcex) )
    {
        MessageBox(NULL,
            L"RegisterClass failed!",
            L"ERROR",
            NULL);

        return 1;
    }


    HWND time_hwnd = CreateWindowEx(
        WS_EX_LAYERED,
        TIMER_WINDOW_CLASS,
        SZ_TITLE,
        WS_POPUP | WSF_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    // ��������ʱ�Ĳ�͸����
    SetLayeredWindowAttributes(time_hwnd, 0, 0, LWA_ALPHA);

    // Բ��
    HRGN h_rgn = CreateRoundRectRgn(
        0, 0,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        10, 10);

    SetWindowRgn(time_hwnd, h_rgn, TRUE);

    SetWindowPos(
        time_hwnd, NULL,
        10, 10,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    SetTimer(time_hwnd, DATE_TIMER_ID, 1000, NULL);

    if ( !time_hwnd )
    {
        MessageBox(NULL,
            L"Call to CreateWindowEx failed!",
            L"ERROR",
            NULL);

        return 1;
    }

    ShowWindow(time_hwnd, nCmdShow);
    UpdateWindow(time_hwnd);

    return 0;
}

/// <summary>
/// �������ô����ಢ��ʾ���ڡ�
/// </summary>
/// <param name="hInstance">Ӧ�ó���ʵ���ľ����</param>
/// <param name="nCmdShow">������ʾ��ʽ�ı�־��</param>
/// <returns>����ɹ��򷵻� 0��ʧ���򷵻� 1��</returns>
int CreateSettingClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow) {
    WNDCLASSEX setting_wcex;

    setting_wcex.cbSize = sizeof(WNDCLASSEX);
    setting_wcex.style = CS_HREDRAW | CS_VREDRAW;
    setting_wcex.lpfnWndProc = NULL; // TODO: 
    setting_wcex.cbClsExtra = 0;
    setting_wcex.cbWndExtra = 0;
    setting_wcex.hInstance = hInstance;
    setting_wcex.hIcon = LoadIcon(setting_wcex.hInstance, IDI_APPLICATION);
    setting_wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    setting_wcex.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
    setting_wcex.lpszMenuName = NULL;
    setting_wcex.lpszClassName = TIMER_WINDOW_CLASS;
    setting_wcex.hIconSm = LoadIcon(setting_wcex.hInstance, IDI_APPLICATION);


    if ( !RegisterClassEx(&setting_wcex) )
    {
        MessageBox(NULL,
            L"RegisterClass failed!",
            L"ERROR",
            NULL);

        return 1;
    }

    HWND setting_hwnd = CreateWindowEx(
        WS_OVERLAPPEDWINDOW,
        TIMER_WINDOW_CLASS,
        SZ_TITLE,
        WS_POPUP | WSF_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if ( !setting_hwnd )
    {
        MessageBox(NULL,
            L"Call to CreateWindowEx failed!",
            L"ERROR",
            NULL);

        return 1;
    }

    return 0;
}


int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow
) {

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    auto ret = CreateTimeClassAndWindow(hInstance, nCmdShow);
    if ( ret > 0 ) {
        return ret;
    }

    MSG msg;

    while ( GetMessage(&msg, NULL, 0, 0) > 0 )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast< int >( msg.wParam );
};

