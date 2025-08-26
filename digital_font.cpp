#include <windows.h>
#include <gdiplus.h>
#include <cmath>
#include <array>
#include "gdi_obj.h"
#include "digital_font.h"
#include "date.h"

/// <summary>
/// ��������壺��GDIʵ��
/// </summary>
namespace DigitalFont {

    using namespace IGDI;
    using namespace Gdiplus;

    // ��׼�߶�����ܶ�������0-6��
    enum class SegmentIndex {
        TOP = 1,       // �Ϻ��
        LEFT_UP,   // ��������
        RIGHT_UP,  // ��������
        MIDDLE,    // �к��
        LEFT_DOWN, // ��������
        RIGHT_DOWN,// ��������
        BOTTOM,    // �º��
    };

    typedef SegmentIndex SI;
    
   // �����߶�����ܵĶ�����
    constexpr std::array<std::array<SI, 7>, 10> DigitSegments = {
        std::array<SI, 7>{SI::TOP, SI::BOTTOM, SI::LEFT_UP, SI::LEFT_DOWN, SI::RIGHT_UP, SI::RIGHT_DOWN},  // 0
        {SI::RIGHT_UP, SI::RIGHT_DOWN},          // 1
        {SI::TOP, SI::MIDDLE, SI::BOTTOM, SI::RIGHT_UP, SI::LEFT_DOWN},    // 2
        {SI::TOP, SI::MIDDLE, SI::BOTTOM, SI::RIGHT_UP, SI::RIGHT_DOWN}, // 3
        {SI::MIDDLE, SI::LEFT_UP, SI::RIGHT_UP, SI::RIGHT_DOWN},      // 4
        {SI::TOP, SI::MIDDLE, SI::BOTTOM, SI::LEFT_UP, SI::RIGHT_DOWN},    // 5
        {SI::TOP, SI::MIDDLE, SI::BOTTOM, SI::LEFT_UP, SI::LEFT_DOWN, SI::RIGHT_DOWN},    // 6
        {SI::TOP, SI::RIGHT_UP, SI::RIGHT_DOWN},        // 7
        {SI::TOP, SI::MIDDLE, SI::BOTTOM, SI::LEFT_UP, SI::LEFT_DOWN, SI::RIGHT_UP, SI::RIGHT_DOWN},  // 8
        {SI::TOP, SI::MIDDLE, SI::BOTTOM, SI::LEFT_UP, SI::RIGHT_UP, SI::RIGHT_DOWN}     // 9
    }; 

    // ���Ƶ����� (����)
    static void drawSegment(HDC hdc, int x, int y, int width, int height) {
        COLORREF c = RGB(0, 0, 0);

        AutoGDI<HBRUSH> brush(CreateSolidBrush(c));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

        Rectangle(hdc, x, y, x + width, y + height);

        SelectObject(hdc, oldBrush);
    }

    // ��������
    void DrawDigit(HDC hdc, int x, int y, int digit) {
        if ( digit < 0 || digit > 9 ) return;
        
        const auto& segments = DigitSegments[digit];

        constexpr int w = SEG_WIDTH;
        constexpr int l = SEG_LENGTH;

        auto draw_segent = [&](int seg_x, int seg_y, int width, int height) -> void {
           drawSegment(hdc, seg_x, seg_y, width, height);
        };

        for (auto& seg : segments) {
            switch (seg) {
            case SI::TOP:
                draw_segent(x + w, y, l, w);
                break;
            case SI::BOTTOM:
                draw_segent(x + w, y + 2*(l+w), l, w);
                break;
            case SI::MIDDLE:
                draw_segent(x + w, y + (l+w), l, w);
                break;
            case SI::LEFT_UP:
                draw_segent(x, y + w, w, l);
                break;
            case SI::LEFT_DOWN:
                draw_segent(x, y + l + 2*w, w, l);
                break;
            case SI::RIGHT_UP:
                draw_segent(x + w + l, y + w, w, l);
                break;
            case SI::RIGHT_DOWN:
                draw_segent(x + w + l, y + l + 2*w, w, l);
                break;
            }
        }
    }

    // ����ʱ��
    void DrawClock(HDC hdc, std::wstring time_str) {

        int start_x = PAINT_START_X;
        int start_y = PAINT_START_Y;

        for ( wchar_t c : time_str ) {
            // ����
            if ( c >= L'0' && c <= L'9' ) {
                int digit = c - L'0';
                DrawDigit(hdc, start_x, start_y, digit);
                // ÿ����������ֵĳ����� SEG_LENGTH + 2*SEG_WIDTH
                start_x += SEG_LENGTH + 2*SEG_WIDTH + DIGIT_SPACING;
            }

            // ð��
            if ( c == L':' ) {
                int colon_y = start_y + (SEG_LENGTH*2 + SEG_WIDTH*3) / 2;
                
                IGDI::AutoGDI<HBRUSH> brush(CreateSolidBrush(RGB(0, 0, 0)));
                HBRUSH old_brash = (HBRUSH)SelectObject(hdc, brush);

                Rectangle(hdc,
                    start_x, colon_y - COLON_SIZE - COLON_OFFSET_Y,
                    start_x + COLON_SIZE, colon_y - COLON_OFFSET_Y);

                Rectangle(hdc,
                    start_x, colon_y + COLON_OFFSET_Y,
                    start_x + COLON_SIZE, colon_y + COLON_SIZE + COLON_OFFSET_Y);
                
                SelectObject(hdc, old_brash);
                start_x += COLON_SIZE + DIGIT_SPACING;
            }
        }
    }
}