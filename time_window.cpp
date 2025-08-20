
#include "time_window.h"
#include "task_sched.h"
#include "date.h"
#include "setting_window.h"
#include "test_window.h"
#include "gdi_obj.h"
#include "digital_font.h"
#include "tray.h"
#include "resource.h"
#include "common.h"

using namespace std::chrono;

struct WindowPos
{
    BOOL is_dragging = FALSE; // 窗口移动状态
    POINT drag_start = {0, 0}; // 鼠标拖动位置起点
};

struct WindowDisplay
{
    BOOL is_show = FALSE; // 窗口是否显示
    UINT alpha = 0; // 透明度
    BOOL fading = FALSE; // 窗口动画标识, 为TRUE时窗口正在进行(消失|进入)动画
};


static void Exit(const HWND hwnd)
{
    KillTimer(hwnd, DATE_TIMER_ID);
    KillTimer(hwnd, ANIMATION_TIMER_ID);
    PostQuitMessage(0);
}

static void timerWindowFadeIn(const HWND hwnd, WindowDisplay& wd)
{
    wd.is_show = TRUE;
    wd.fading = TRUE;
    SetTimer(hwnd, ANIMATION_TIMER_ID, 10, nullptr); // 启动新定时器
}

static void timerWindowFadeOut(const HWND hwnd, WindowDisplay& wd)
{
    wd.is_show = FALSE;
    wd.fading = TRUE;
    SetTimer(hwnd, ANIMATION_TIMER_ID, 10, nullptr); // 启动新定时器
}

static void updateTimerWindowFadeAnimation(const HWND hwnd, WindowDisplay& wd)
{
    // 提醒窗口开始动画
    if (wd.fading && wd.is_show)
    {
        wd.alpha += FADE_DURATION;
        if (wd.alpha >= 255)
        {
            wd.alpha = 255;
            wd.fading = FALSE;
            // 动画结束关闭动画定时器
            KillTimer(hwnd, ANIMATION_TIMER_ID);
        }
    }

    // 提醒窗口结束动画
    if (wd.fading && FALSE == wd.is_show)
    {
        wd.alpha -= FADE_DURATION;
        if (wd.alpha <= 0)
        {
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

    // 窗口位置状态
    static WindowPos g_window_pos;
    // 窗口显示状态
    static WindowDisplay g_window_display;

    // 计步器
    static StepCountDown g_step_down(Date::NextHourDistance());

    switch (message)
    {
    case WM_CREATE:
        {
            Tray::AddIcon(hWnd);
            break;
        }

    case Tray::WM_ICON:
        {
            if (lParam == WM_RBUTTONUP)
            {
                Tray::ShowMenu(hWnd);
            }

            if (lParam == WM_LBUTTONUP)
            {
                g_window_display.alpha = 255;
                timerWindowFadeOut(hWnd, g_window_display);
            }
            break;
        }

    case WM_TIMER:
        {
            if (wParam == DATE_TIMER_ID)
            {
                g_step_down.step();

                // 如果距离已经在提醒半径内且提醒未激活：则可以开始提醒
                if (g_step_down.distance() <= WINDOWS_SHOW_TIME_RADIUS_SEC && FALSE == g_window_display.is_show)
                {
                    timerWindowFadeIn(hWnd, g_window_display);
                }

                // 如果提醒激活，且距离已经超出提醒半径：则停止提醒，重置距离计算
                if (g_step_down.distance() > WINDOWS_SHOW_TIME_RADIUS_SEC && g_window_display.is_show)
                {
                    timerWindowFadeOut(hWnd, g_window_display);
                    // 重新计算下一次时间
                    g_step_down.reset(Date::NextHourDistance());
                }

                InvalidateRect(hWnd, nullptr, TRUE);
            }

            if (wParam == ANIMATION_TIMER_ID)
            {
                updateTimerWindowFadeAnimation(hWnd, g_window_display);
            }
            break;
        }

    // 鼠标按下
    case WM_LBUTTONDOWN:
        {
            // 鼠标位置
            POINT pt = {LOWORD(lParam), HIWORD(lParam)};

            RECT rect;
            // 获得窗口矩形
            GetClientRect(hWnd, &rect);
            RECT area_rect = rect;

            // 检查鼠标位置是否在窗口矩形内
            if (PtInRect(&area_rect, pt))
            {
                g_window_pos.drag_start = pt;
                g_window_pos.is_dragging = TRUE;

                // 设置鼠标捕获以及修改鼠标样式
                SetCapture(hWnd);
                SetCursor(LoadCursor(NULL, IDC_SIZEALL));
            }

            break;
        }

    case WM_LBUTTONUP:
        {
            if (g_window_pos.is_dragging)
            {
                g_window_pos.is_dragging = FALSE;
                ReleaseCapture();
            }

            break;
        }

    case WM_MOUSEMOVE:
        {
            if (g_window_pos.is_dragging)
            {
                POINT pt;
                GetCursorPos(&pt);

                RECT rect;
                GetWindowRect(hWnd, &rect);

                int new_x = rect.left + (pt.x - (rect.left + g_window_pos.drag_start.x));
                int new_y = rect.top + (pt.y - (rect.top + g_window_pos.drag_start.y));

                // 移动窗口
                SetWindowPos(hWnd, NULL, new_x, new_y, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }

    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case Tray::TRAY_MENU_SETTING:
                {
                    auto h_instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
                    CreateSettingClassAndWindow(h_instance, SW_SHOWNORMAL);
                    break;
                }

            case Tray::TRAY_MENU_TEST:
                {
                    auto h_instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
                    CreateTestClassAndWindow(h_instance, SW_SHOWNORMAL);
                    break;
                }

            case Tray::TRAY_MENU_STARTUP:
                {
                    std::wstring current_exec_path;
                    const auto is_active_startup = TaskSched::IsActiveStartupTask(&current_exec_path);

                    if (is_active_startup)
                    {
                        TaskSched::DeleteStartupTask();
                    } else
                    {
                        TaskSched::CreateStartupTask();
                    }
                    break;
                }

            case Tray::TRAY_MENU_EXIT:
                {
                    Exit(hWnd);
                    break;
                }
            }
        }

    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);
            auto time_str = Date::CurrTimeWStr();

            RECT client_rect;
            GetClientRect(hWnd, &client_rect);

            // 绘制背景不填充
            const int old_bk_mode = SetBkMode(hdc, TRANSPARENT);

            RECT time_rect;
            time_rect.left = client_rect.left + TIMER_WINDOW_MARGIN;
            time_rect.top = client_rect.top + TIMER_WINDOW_MARGIN;
            time_rect.right = time_rect.left + (client_rect.right - client_rect.left) - TIMER_WINDOW_MARGIN * 2;
            time_rect.bottom = time_rect.top + (client_rect.bottom - client_rect.top) - TIMER_WINDOW_MARGIN * 2;

            IGDI::AutoGDI<HBRUSH> brush(CreateSolidBrush(RGB(220, 220, 220)));
            IGDI::AutoGDI<HPEN> pen(CreatePen(PS_SOLID, 0, RGB(220, 220, 220)));

            const HBRUSH old_brush = (HBRUSH)SelectObject(hdc, brush);
            const HPEN old_pen = (HPEN)SelectObject(hdc, pen);

            RoundRect(
                hdc, time_rect.left, time_rect.top, time_rect.right, time_rect.bottom,
                TIMER_DISPLAY_ROUND_W, TIMER_DISPLAY_ROUND_H);

            //// 设置回旧
            SelectObject(hdc, old_brush);
            SelectObject(hdc, old_pen);
            SetBkMode(hdc, old_bk_mode);

            // 渲染时间
            DigitalFont::DrawClock(hdc, time_str);


            EndPaint(hWnd, &ps);
            break;
        }

    case WM_CLOSE:
        {
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

void registerTimeClass(_In_ HINSTANCE hInstance)
{
    WNDCLASSEX time_wcex;

    time_wcex.cbSize = sizeof(WNDCLASSEX);
    time_wcex.style = CS_HREDRAW | CS_VREDRAW;
    time_wcex.lpfnWndProc = timeWndProc;
    time_wcex.cbClsExtra = 0;
    time_wcex.cbWndExtra = 0;
    time_wcex.hInstance = hInstance;
    time_wcex.hIcon = LoadIcon(time_wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));
    time_wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    time_wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    time_wcex.lpszMenuName = NULL;
    time_wcex.lpszClassName = TIMER_CLASS_NAME;
    time_wcex.hIconSm = LoadIcon(time_wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

    RegisterClassEx(&time_wcex);
    Debug::TryLastErrorMessageBox();
}

/// <summary>
/// 创建一个定时器窗口类并显示窗口，设置窗口样式、圆角和定时器。
/// </summary>
/// <param name="hInstance">应用程序实例句柄。</param>
/// <param name="nCmdShow">窗口显示方式。</param>
/// <returns>如果成功则返回0，如果失败则返回1。</returns>
int CreateTimeClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow)
{
    WNDCLASSEX time_wcex = {sizeof(WNDCLASSEX)};
    if (FALSE == GetClassInfoEx(hInstance, TIMER_CLASS_NAME, &time_wcex))
    {
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

    // 设置启动时的不透明度
    SetLayeredWindowAttributes(time_hwnd, 0, 0, LWA_ALPHA);

    // 圆角
    HRGN h_rgn = CreateRoundRectRgn(
        TIMER_WINDOW_ROUND_X, TIMER_WINDOW_ROUND_Y,
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
