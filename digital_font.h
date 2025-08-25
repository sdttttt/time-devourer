#pragma once

#include <windows.h>
#include <sstream>

namespace DigitalFont {
    constexpr int SEG_WIDTH = 4;      // 数码管段的宽度
    constexpr int SEG_LENGTH = 30;    // 数码管段的长度
    constexpr int DIGIT_SPACING = 10; // 数字之间的间距

    constexpr int PAINT_START_X = 20; // 绘制的起始坐标X
    constexpr int PAINT_START_Y = 20; // 绘制的起始坐标Y

    constexpr int COLON_SIZE = 5;     // 冒号方块大小
    constexpr int COLON_GAP = 20;     // 冒号之间的间距
    constexpr int COLON_OFFSET_Y = 8; // 冒号的偏移量


    void DrawDigit(HDC hdc, int x, int y, int digit);
    void DrawClock(HDC hdc, PAINTSTRUCT& ps, std::wstring time_str);
}