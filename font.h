#pragma once

#include <windows.h>
#include <string>

constexpr auto FONT_FAMILY = L"Arial";

namespace Font {
    void DrawScaledText(HDC hdc, _In_ RECT& rect, _In_ const std::wstring& str);
}
