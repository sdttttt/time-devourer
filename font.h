#pragma once

#include <windows.h>
#include <string>


constexpr wchar_t FONT_FAMILY[] = L"Arial";

namespace Font {
	void DrawScaledText(HDC hdc, const RECT& rect, const std::wstring& str);
}