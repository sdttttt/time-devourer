#pragma once

#include <windows.h>

// ���ô�����
constexpr TCHAR SETTING_WINDOW_CLASS[ ] = L"SettingWindowClass";

// ��������
constexpr TCHAR SETTING_WINDOW_TITLE[ ] = L"Setting";

// ���ڳߴ�
constexpr UINT SETTING_WINDOW_WIDTH = 500;
constexpr UINT SETTING_WINDOW_HEIGHT = 500;

int CreateSettingClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow);