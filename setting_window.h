#pragma once

#include <windows.h>

// ���ô�����
constexpr const TCHAR* TEST_CLASS_NAME = L"SettingWindowClass";

// ��������
constexpr const TCHAR* TEST_WINDOW_TITLE = L"Setting";

// ���ڳߴ�
constexpr UINT TEST_WINDOW_WIDTH = 500;
constexpr UINT TEST_WINDOW_HEIGHT = 500;

int CreateSettingClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow);