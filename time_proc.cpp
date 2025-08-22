#include <windows.h>
#include "tray.h"
#include "date.h"
#include "font.h"
#include "resource.h"

#include "time_proc.h"


void Exit(HWND hwnd) {
    KillTimer(hwnd, DATE_TIMER_ID);
    KillTimer(hwnd, ANIMATION_TIMER_ID);
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

    // 窗口是否显示
    static BOOL is_show = FALSE;

    // 窗口移动状态
    static BOOL is_dragging = FALSE;
    // 鼠标拖动位置起点
    static POINT drag_start;

    // 透明度
    static int g_alpha = 255;
    // 窗口动画标识, 为TRUE时窗口正在进行(消失|进入)动画
    static BOOL g_fading = FALSE;
    // 动画速度
    constexpr int FADE_DURATION = 5;

    switch ( message )
    {
    case WM_CREATE: {
        HICON h_icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
        Tray::AddIcon(hWnd, h_icon);
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

                 // 鼠标按下
    case WM_LBUTTONDOWN: {
        // 鼠标位置
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        RECT rect;
        // 获得窗口矩形
        GetClientRect(hWnd, &rect);
        RECT area_rect = rect;

        // 检查鼠标位置是否在窗口矩形内
        if ( PtInRect(&area_rect, pt) ) {
            drag_start = pt;
            is_dragging = TRUE;

            // 设置鼠标捕获以及修改鼠标样式
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

            // 移动窗口
            SetWindowPos(hWnd, NULL, new_x, new_y, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    case WM_COMMAND: {
        switch ( LOWORD(wParam) ) {
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
