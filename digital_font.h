#pragma once

#include <windows.h>
#include <sstream>

namespace DigitalFont {
    constexpr auto SEG_WIDTH = 4;      // ����ܶεĿ��
    constexpr auto SEG_LENGTH = 25;    // ����ܶεĳ���
    constexpr auto DIGIT_SPACING = 10; // ����֮��ļ��

    constexpr auto PAINT_START_X = 20; // ���Ƶ���ʼ����X
    constexpr auto PAINT_START_Y = 20; // ���Ƶ���ʼ����Y

    constexpr auto COLON_SIZE = 5;     // ð�ŷ����С
    constexpr auto COLON_GAP = 20;     // ð��֮��ļ��
    constexpr auto COLON_OFFSET_Y = 8; // ð�ŵ�ƫ����


    void DrawDigit(HDC hdc, int x, int y, int digit);
    void DrawClock(HDC hdc, std::wstring time_str);
}