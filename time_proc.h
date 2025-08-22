#pragma once

#include <windows.h>


// 时间的更新定时器
constexpr UINT_PTR DATE_TIMER_ID = 0x0001;
// 动画的更新定时器
constexpr UINT_PTR ANIMATION_TIMER_ID = 0x0002;

// 窗口显示时间半径: 秒
constexpr UINT WINDOWS_SHOW_TIME_RADIUS_SEC = 30;

LRESULT CALLBACK WndProc(
    _In_ HWND hWnd,
    _In_ UINT message,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
);