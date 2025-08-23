#include <windows.h>
#include "tray.h"
#include "date.h"
#include "font.h"
#include "resource.h"

#include "time_window.h"
#include "setting_window.h"
#include "common.h"


static void Exit(HWND hwnd) {
    KillTimer(hwnd, DATE_TIMER_ID);
    KillTimer(hwnd, ANIMATION_TIMER_ID);
    PostQuitMessage(0);
}

LRESULT CALLBACK timeWndProc(
    _In_ HWND hWnd,
    _In_ UINT message,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    PAINTSTRUCT ps;
    HDC hdc;

    // �����Ƿ���ʾ
    static BOOL is_show = FALSE;

    // �����ƶ�״̬
    static BOOL is_dragging = FALSE;
    // ����϶�λ�����
    static POINT drag_start;

    // ͸����
    static int g_alpha = 255;
    // ���ڶ�����ʶ, ΪTRUEʱ�������ڽ���(��ʧ|����)����
    static BOOL g_fading = FALSE;
    // �����ٶ�
    constexpr int FADE_DURATION = 5;

    switch ( message )
    {
    case WM_CREATE: {
        Tray::AddIcon(hWnd);
        break;
    }

    case Tray::WM_ICON: {
        if ( lParam == WM_RBUTTONUP ) {
            Tray::ShowMenu(hWnd);
        }

        if ( lParam == WM_LBUTTONDBLCLK ) {
            ShowWindow(hWnd, SW_SHOW);
            SetForegroundWindow(hWnd);
        }
    }

    case WM_TIMER: {
        if ( wParam == DATE_TIMER_ID )
        {
            auto next_hour_sec = Date::NextHourDistance();
            auto max = max(next_hour_sec, std::chrono::seconds(WINDOWS_SHOW_TIME_RADIUS_SEC));
            auto min = min(next_hour_sec, std::chrono::seconds(WINDOWS_SHOW_TIME_RADIUS_SEC));
            auto distance = max - min;
            if ( distance < std::chrono::seconds(WINDOWS_SHOW_TIME_RADIUS_SEC) )
            {
                if ( is_show == FALSE ) {
                    SetTimer(hWnd, ANIMATION_TIMER_ID, 50, NULL);
                    is_show = TRUE;
                    g_fading = TRUE;
                }
            }
            else {
                if ( is_show ) {
                    SetTimer(hWnd, ANIMATION_TIMER_ID, 50, NULL);
                    is_show = FALSE;
                    g_fading = TRUE;
                }
            }

            InvalidateRect(hWnd, NULL, FALSE);
        }

        if ( wParam == ANIMATION_TIMER_ID )
        {
            if ( g_fading && is_show ) {
                g_alpha += FADE_DURATION;
                if ( g_alpha >= 255 ) {
                    g_alpha = 255;
                    g_fading = FALSE;
                    KillTimer(hWnd, ANIMATION_TIMER_ID);
                }
            }
            if ( g_fading && is_show == FALSE ) {
                g_alpha -= FADE_DURATION;
                if ( g_alpha <= 0 ) {
                    g_alpha = 0;
                    g_fading = TRUE;
                    KillTimer(hWnd, ANIMATION_TIMER_ID);
                }
            }

            SetLayeredWindowAttributes(hWnd, 0, g_alpha, LWA_ALPHA);
        }
        break;
    }

    // ��갴��
    case WM_LBUTTONDOWN: {
        // ���λ��
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        RECT rect;
        // ��ô��ھ���
        GetClientRect(hWnd, &rect);
        RECT area_rect = rect;

        // ������λ���Ƿ��ڴ��ھ�����
        if ( PtInRect(&area_rect, pt) ) {
            drag_start = pt;
            is_dragging = TRUE;

            // ������겶���Լ��޸������ʽ
            SetCapture(hWnd);
            SetCursor(LoadCursor(NULL, IDC_SIZEALL));
        }

        break;
    }

    case WM_LBUTTONUP: {
        if ( is_dragging ) {
            is_dragging = FALSE;
            ReleaseCapture();
        }

        break;
    }

    case WM_MOUSEMOVE: {
        if ( is_dragging ) {
            POINT pt;
            GetCursorPos(&pt);

            RECT rect;
            GetWindowRect(hWnd, &rect);

            int new_x = rect.left + ( pt.x - ( rect.left + drag_start.x ) );
            int new_y = rect.top + ( pt.y - ( rect.top + drag_start.y ) );

            // �ƶ�����
            SetWindowPos(hWnd, NULL, new_x, new_y, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    case WM_COMMAND: {
        switch ( LOWORD(wParam) ) {
        case Tray::TRAY_MENU_SETTING: {
            auto hinst = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
            CreateSettingClassAndWindow(hinst, SW_SHOWNORMAL);
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

void registerTimeClass(_In_ HINSTANCE hInstance) {
    WNDCLASSEX time_wcex;

    time_wcex.cbSize = sizeof(WNDCLASSEX);
    time_wcex.style = CS_HREDRAW | CS_VREDRAW;
    time_wcex.lpfnWndProc = timeWndProc;
    time_wcex.cbClsExtra = 0;
    time_wcex.cbWndExtra = 0;
    time_wcex.hInstance = hInstance;
    time_wcex.hIcon = LoadIcon(time_wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
    time_wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    time_wcex.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
    time_wcex.lpszMenuName = NULL;
    time_wcex.lpszClassName = TIMER_CLASS_NAME;
    time_wcex.hIconSm = LoadIcon(time_wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    RegisterClassEx(&time_wcex);
    Debug::TryLastErrorMessageBox();
}

/// <summary>
/// ����һ����ʱ�������ಢ��ʾ���ڣ����ô�����ʽ��Բ�ǺͶ�ʱ����
/// </summary>
/// <param name="hInstance">Ӧ�ó���ʵ�������</param>
/// <param name="nCmdShow">������ʾ��ʽ��</param>
/// <returns>����ɹ��򷵻�0�����ʧ���򷵻�1��</returns>
int CreateTimeClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow) {
    
    WNDCLASSEX time_wcex = { sizeof(WNDCLASSEX)};
    if ( FALSE == GetClassInfoEx(hInstance, TIMER_CLASS_NAME, &time_wcex) ) {
        registerTimeClass(hInstance);
    }

    HWND time_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        TIMER_CLASS_NAME,
        TIMER_WINDOW_TITLE,
        WS_POPUP | WSF_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        TIMER_WINDOW_WIDTH, TIMER_WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    Debug::TryLastErrorMessageBox();

    // ��������ʱ�Ĳ�͸����
    SetLayeredWindowAttributes(time_hwnd, 0, 0, LWA_ALPHA);

    // Բ��
    HRGN h_rgn = CreateRoundRectRgn(
        0, 0,
        TIMER_WINDOW_WIDTH, TIMER_WINDOW_HEIGHT,
        10, 10);

    SetWindowRgn(time_hwnd, h_rgn, TRUE);

    SetWindowPos(
        time_hwnd, NULL,
        10, 10,
        TIMER_WINDOW_WIDTH, TIMER_WINDOW_HEIGHT,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    SetTimer(time_hwnd, DATE_TIMER_ID, 1000, NULL);

    ShowWindow(time_hwnd, nCmdShow);
    UpdateWindow(time_hwnd);

    return 0;
}