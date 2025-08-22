#pragma once

#include <windows.h>
#include <string>


constexpr wchar_t FONT_FAMILY[ ] = L"Arial";

namespace Font {
    void DrawScaledText(HDC hdc, _In_ const RECT& rect, _In_ const std::wstring& str);
}