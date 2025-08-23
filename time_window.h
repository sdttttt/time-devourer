#pragma once

#include <windows.h>
#include <chrono>

// ʱ����ʾ������
constexpr TCHAR TIMER_CLASS_NAME[ ] = L"TimeWindowClass";

// ��������
constexpr TCHAR TIMER_WINDOW_TITLE[ ] = L"Timer";

// ���ڳߴ�
constexpr UINT TIMER_WINDOW_WIDTH = 400;
constexpr UINT TIMER_WINDOW_HEIGHT = 100;

// ʱ��ĸ��¶�ʱ��
constexpr UINT_PTR DATE_TIMER_ID = 0x0001;
// �����ĸ��¶�ʱ��
constexpr UINT_PTR ANIMATION_TIMER_ID = 0x0002;

// ������ʾʱ��뾶: ��
constexpr UINT WINDOWS_SHOW_TIME_RADIUS_SEC = 30;

// �����ĵ��뵭��ʱ��
constexpr UINT FADE_DURATION = std::chrono::milliseconds(1).count();

int CreateTimeClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow);
