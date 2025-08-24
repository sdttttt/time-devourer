#pragma once

#include "windows.h"

namespace DigitalFont {
    constexpr int SEG_WIDTH = 4;      // 数码管段的宽度
    constexpr int SEG_LENGTH = 40;    // 数码管段的长度
    constexpr int DIGIT_SPACING = 10; // 数字之间的间距
    constexpr int COLON_SIZE = 6;     // 冒号方块大小
    constexpr int COLON_GAP = 10;     // 冒号之间的间距


    void DrawDigit(HDC hdc, int x, int y, int digit);
}