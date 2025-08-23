#include <windows.h>
#include "tray.h"
#include "date.h"
#include "font.h"
#include "resource.h"

#include "time_window.h"
#include "setting_window.h"
#include "common.h"

using namespace std::chrono;

struct WindowPos {
    BOOL is_dragging = FALSE; // �����ƶ�״̬
    POINT drag_start = { 0, 0 }; // ����϶�λ�����
};

struct WindowDisplay {
    BOOL is_show = FALSE;  // �����Ƿ���ʾ
    UINT alpha = 0; // ͸����
    BOOL fading = FALSE; // ���ڶ�����ʶ, ΪTRUEʱ�������ڽ���(��ʧ|����)����
};


static void Exit(HWND hwnd) {
    KillTimer(hwnd, DATE_TIMER_ID);
    KillTimer(hwnd, ANIMATION_TIMER_ID);
    PostQuitMessage(0);
}

static void timerWindowFadeIn(HWND hwnd, WindowDisplay& wd) {
    wd.is_show = TRUE;
    wd.fading = TRUE;
    SetTimer(hwnd, ANIMATION_TIMER_ID, 10, NULL); // �����¶�ʱ��
}

static void timerWindowFadeOut(HWND hwnd, WindowDisplay& wd) {
    wd.is_show = FALSE;
    wd.fading = TRUE;
    SetTimer(hwnd, ANIMATION_TIMER_ID, 10, NULL); // �����¶�ʱ��
}

static void updateTimerWindowFadeAnimation(HWND hwnd, WindowDisplay& wd) {
    // ���Ѵ��ڿ�ʼ����
    if ( wd.fading && wd.is_show ) {
        wd.alpha += FADE_DURATION;
        if ( wd.alpha >= 255 ) {
            wd.alpha = 255;
            wd.fading = FALSE;
            // ���������رն�����ʱ��
            KillTimer(hwnd, ANIMATION_TIMER_ID);
        }
    }

    // ���Ѵ��ڽ�������
    if ( wd.fading && FALSE == wd.is_show ) {
        wd.alpha -= FADE_DURATION;
        if ( wd.alpha <= 0 ) {
            wd.alpha = 0;
            wd.fading = TRUE;
            KillTimer(hwnd, ANIMATION_TIMER_ID);
        }
    }

    SetLayeredWindowAttributes(hwnd, 0, wd.alpha, LWA_ALPHA);
}

static LRESULT CALLBACK timeWndProc(
    _In_ HWND hWnd,
    _In_ UINT message,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    PAINTSTRUCT ps;
    HDC hdc;

    // ����λ��״̬
    static WindowPos g_window_pos;
    // ������ʾ״̬
    static WindowDisplay g_window_display;
    
    // �Ʋ���
    static StepCountDown g_step_down(Date::NextHourDistance());
    
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

        if ( lParam == WM_LBUTTONUP ) {
            g_window_display.alpha = 255;
            timerWindowFadeOut(hWnd, g_window_display);
        }
        break;
    }

    case WM_TIMER: {
        if ( wParam == DATE_TIMER_ID )
        {   
            g_step_down.step();

            // ��������Ѿ������Ѱ뾶��������δ�������Կ�ʼ����
            if ( g_step_down.distance() <= WINDOWS_SHOW_TIME_RADIUS_SEC && FALSE == g_window_display.is_show  )
            {
                timerWindowFadeIn(hWnd, g_window_display);
            }

            // ������Ѽ���Ҿ����Ѿ��������Ѱ뾶����ֹͣ���ѣ����þ������
            if ( g_step_down.distance() > WINDOWS_SHOW_TIME_RADIUS_SEC && g_window_display.is_show )
            {
                timerWindowFadeOut(hWnd, g_window_display);
                // ���¼�����һ��ʱ��
                g_step_down.reset(Date::NextHourDistance());
             }

            InvalidateRect(hWnd, NULL, FALSE);
        }

        if ( wParam == ANIMATION_TIMER_ID )
        {
            updateTimerWindowFadeAnimation(hWnd, g_window_display);
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
            g_window_pos.drag_start = pt;
            g_window_pos.is_dragging = TRUE;

            // ������겶���Լ��޸������ʽ
            SetCapture(hWnd);
            SetCursor(LoadCursor(NULL, IDC_SIZEALL));
        }

        break;
    }

    case WM_LBUTTONUP: {
        if ( g_window_pos.is_dragging ) {
            g_window_pos.is_dragging = FALSE;
            ReleaseCapture();
        }

        break;
    }

    case WM_MOUSEMOVE: {
        if ( g_window_pos.is_dragging ) {
            POINT pt;
            GetCursorPos(&pt);

            RECT rect;
            GetWindowRect(hWnd, &rect);

            int new_x = rect.left + ( pt.x - ( rect.left + g_window_pos.drag_start.x ) );
            int new_y = rect.top + ( pt.y - ( rect.top + g_window_pos.drag_start.y ) );

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
    case WM_CLOSE: {
        DestroyWindow(hWnd);
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
    time_wcex.hIcon = LoadIcon(time_wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));
    time_wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    time_wcex.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
    time_wcex.lpszMenuName = NULL;
    time_wcex.lpszClassName = TIMER_CLASS_NAME;
    time_wcex.hIconSm = LoadIcon(time_wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

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
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
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
        TIMER_WINDOW_X, TIMER_WINDOW_Y,
        TIMER_WINDOW_WIDTH, TIMER_WINDOW_HEIGHT,
        TIMER_WINDOW_ROUND_W, TIMER_WINDOW_ROUND_H);

    SetWindowRgn(time_hwnd, h_rgn, TRUE);
    
    SetWindowPos(
        time_hwnd, NULL,
        TIMER_WINDOW_X, TIMER_WINDOW_Y,
        TIMER_WINDOW_WIDTH, TIMER_WINDOW_HEIGHT,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    SetTimer(time_hwnd, DATE_TIMER_ID, 1000, NULL);

    ShowWindow(time_hwnd, nCmdShow);
    UpdateWindow(time_hwnd);

    return 0;
}