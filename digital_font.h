#pragma once

#include "windows.h"

namespace DigitalFont {
    constexpr int SEG_WIDTH = 4;      // ����ܶεĿ��
    constexpr int SEG_LENGTH = 40;    // ����ܶεĳ���
    constexpr int DIGIT_SPACING = 10; // ����֮��ļ��
    constexpr int COLON_SIZE = 6;     // ð�ŷ����С
    constexpr int COLON_GAP = 10;     // ð��֮��ļ��


    void DrawDigit(HDC hdc, int x, int y, int digit);
}