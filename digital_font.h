#pragma once

#include <windows.h>
#include <sstream>

namespace DigitalFont {
    constexpr int SEG_WIDTH = 4;      // ����ܶεĿ��
    constexpr int SEG_LENGTH = 30;    // ����ܶεĳ���
    constexpr int DIGIT_SPACING = 10; // ����֮��ļ��

    constexpr int PAINT_START_X = 20; // ���Ƶ���ʼ����X
    constexpr int PAINT_START_Y = 20; // ���Ƶ���ʼ����Y

    constexpr int COLON_SIZE = 5;     // ð�ŷ����С
    constexpr int COLON_GAP = 20;     // ð��֮��ļ��
    constexpr int COLON_OFFSET_Y = 8; // ð�ŵ�ƫ����


    void DrawDigit(HDC hdc, int x, int y, int digit);
    void DrawClock(HDC hdc, PAINTSTRUCT& ps, std::wstring time_str);
}