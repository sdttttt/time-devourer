#pragma once

#include <windows.h>

// ���ô�����
constexpr auto TEST_CLASS_NAME = L"SettingWindowClass";

// ��������
constexpr auto TEST_WINDOW_TITLE = L"Setting";

// ���ڳߴ�
constexpr auto TEST_WINDOW_WIDTH = 500;
constexpr auto TEST_WINDOW_HEIGHT = 500;

int CreateSettingClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow);