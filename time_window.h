#pragma once

#include <windows.h>
#include <chrono>

// 时间显示窗口类
constexpr TCHAR TIMER_CLASS_NAME[ ] = L"TimeWindowClass";

// 标题栏名
constexpr TCHAR TIMER_WINDOW_TITLE[ ] = L"Timer";

// 窗口尺寸
constexpr unsigned int TIMER_WINDOW_WIDTH = 400;
constexpr unsigned int TIMER_WINDOW_HEIGHT = 100;

// 时间的更新定时器
constexpr UINT_PTR DATE_TIMER_ID = 0x0001;
// 动画的更新定时器
constexpr UINT_PTR ANIMATION_TIMER_ID = 0x0002;

// 窗口显示时间半径: 秒
constexpr unsigned int WINDOWS_SHOW_TIME_RADIUS_SEC = 30;

// 动画的淡入淡出时间
constexpr int FADE_DURATION = 1;

int CreateTimeClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow);
