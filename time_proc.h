#pragma once

#include <windows.h>


// ʱ��ĸ��¶�ʱ��
constexpr UINT_PTR DATE_TIMER_ID = 0x0001;
// �����ĸ��¶�ʱ��
constexpr UINT_PTR ANIMATION_TIMER_ID = 0x0002;

// ������ʾʱ��뾶: ��
constexpr UINT WINDOWS_SHOW_TIME_RADIUS_SEC = 30;

LRESULT CALLBACK WndProc(
    _In_ HWND hWnd,
    _In_ UINT message,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
);