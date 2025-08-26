#pragma once

#include <windows.h>
#include <sstream>

namespace DigitalFont {
    constexpr auto SEG_WIDTH = 4;      // 数码管段的宽度
    constexpr auto SEG_LENGTH = 25;    // 数码管段的长度
    constexpr auto DIGIT_SPACING = 10; // 数字之间的间距

    constexpr auto PAINT_START_X = 20; // 绘制的起始坐标X
    constexpr auto PAINT_START_Y = 20; // 绘制的起始坐标Y

    constexpr auto COLON_SIZE = 5;     // 冒号方块大小
    constexpr auto COLON_GAP = 20;     // 冒号之间的间距
    constexpr auto COLON_OFFSET_Y = 8; // 冒号的偏移量


    void DrawDigit(HDC hdc, int x, int y, int digit);
    void DrawClock(HDC hdc, std::wstring time_str);
}