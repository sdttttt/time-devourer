#pragma once

#include <windows.h>
#include <string>

void DrawScaledText(HDC hdc, const RECT& rect, const std::wstring& str);