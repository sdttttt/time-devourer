#pragma once

#include <windows.h>

// 设置窗口类
constexpr const TCHAR* TEST_CLASS_NAME = L"SettingWindowClass";

// 标题栏名
constexpr const TCHAR* TEST_WINDOW_TITLE = L"Setting";

// 窗口尺寸
constexpr UINT TEST_WINDOW_WIDTH = 500;
constexpr UINT TEST_WINDOW_HEIGHT = 500;

int CreateSettingClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow);