#include <windows.h>
#include <gdiplus.h>
#include <cmath>
#include <vector>
#include "gdi_obj.h"
#include "digital_font.h"

/// <summary>
/// ��������壺��GDIʵ��
/// </summary>
namespace DigitalFont {

    using namespace IGDI;
    using namespace Gdiplus;

    // ��׼�߶�����ܶ�������0-6��
    enum class SegmentIndex {
        TOP,       // �Ϻ��
        LEFT_UP,   // ��������
        RIGHT_UP,  // ��������
        MIDDLE,    // �к��
        LEFT_DOWN, // ��������
        RIGHT_DOWN,// ��������
        BOTTOM,    // �º��
    };

    typedef SegmentIndex SI;
    
   // �����߶�����ܵĶ�����
    static const std::vector<std::vector<SI>> DigitSegments = {
        {SI::TOP, SI::BOTTOM, SI::LEFT_UP, SI::LEFT_DOWN, SI::RIGHT_UP, SI::RIGHT_DOWN},  // 0
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

    void DrawDigit(HDC hdc, int x, int y, int digit) {
        if ( digit < 0 || digit > 9 ) return;
        
        const std::vector<SI>& segments = DigitSegments[digit];

        constexpr int w = SEG_WIDTH;
        constexpr int l = SEG_LENGTH;

        auto draw_segent = [&](int seg_x, int seg_y, int width, int height) -> void {
           drawSegment(hdc, seg_x, seg_y, width, height);
        };

        for (auto seg : segments) {
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
}