#include <windows.h>
#include <gdiplus.h>
#include <cmath>
#include <array>
#include "gdi_obj.h"
#include "digital_font.h"
#include "date.h"

/// <summary>
/// 数码管字体：纯GDI实现
/// </summary>
namespace DigitalFont {

    using namespace IGDI;
    using namespace Gdiplus;

    // 标准七段数码管段索引（0-6）
    enum class SegmentIndex {
        TOP = 1,       // 上横段
        LEFT_UP,   // 左上竖段
        RIGHT_UP,  // 右上竖段
        MIDDLE,    // 中横段
        LEFT_DOWN, // 左下竖段
        RIGHT_DOWN,// 右下竖段
        BOTTOM,    // 下横段
    };

    typedef SegmentIndex SI;

    // 定义七段数码管的段索引
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

    /// <summary>
    /// 绘制一个数字
    /// </summary>
    /// <param name="hdc"></param>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <param name="grey"></param>
    static void drawSegment(HDC hdc, int x, int y, int width, int height, BOOL grey) {
        const COLORREF c = grey == FALSE ? RGB(0, 0, 0) : RGB(200, 200, 200);

        const AutoGDI<HBRUSH> brush(CreateSolidBrush(c));
        const AutoGDI<HPEN> pen(CreatePen(PS_SOLID, 0, c));
        const HBRUSH oldBrush = ( HBRUSH ) SelectObject(hdc, brush);
        const HPEN oldPen = ( HPEN ) SelectObject(hdc, pen);

        Rectangle(hdc, x, y, x + width, y + height);

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
    }

    /// <summary>
    /// 绘制一个数字
    /// </summary>
    /// <param name="hdc"></param>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="digit"></param>
    void DrawDigit(HDC hdc, int x, int y, int digit) {
        if ( digit < 0 || digit > 9 ) return;

        const auto& segments = DigitSegments[ digit ];

        constexpr int w = SEG_WIDTH;
        constexpr int l = SEG_LENGTH;

        const auto draw_segent = [&](int seg_x, int seg_y, int width, int height) -> void {
            drawSegment(hdc, seg_x, seg_y, width, height, FALSE);
            };

        const auto has_top = std::find(segments.begin(), segments.end(), SI::TOP) != segments.end();
        const auto has_bottom = std::find(segments.begin(), segments.end(), SI::BOTTOM) != segments.end();
        const auto has_middle = std::find(segments.begin(), segments.end(), SI::MIDDLE) != segments.end();
        const auto has_left_up = std::find(segments.begin(), segments.end(), SI::LEFT_UP) != segments.end();
        const auto has_right_up = std::find(segments.begin(), segments.end(), SI::RIGHT_UP) != segments.end();
        const auto has_left_down = std::find(segments.begin(), segments.end(), SI::LEFT_DOWN) != segments.end();
        const auto has_right_down = std::find(segments.begin(), segments.end(), SI::RIGHT_DOWN) != segments.end();

        if ( has_top ) draw_segent(x + w, y, l, w);
        if ( has_bottom ) draw_segent(x + w, y + 2 * ( l + w ), l, w);
        if ( has_middle ) draw_segent(x + w, y + ( l + w ), l, w);
        if ( has_left_up ) draw_segent(x, y + w, w, l);
        if ( has_left_down ) draw_segent(x, y + l + 2 * w, w, l);
        if ( has_right_up ) draw_segent(x + w + l, y + w, w, l);
        if ( has_right_down ) draw_segent(x + w + l, y + l + 2 * w, w, l);
    }

    /// <summary>
    /// 绘制一个数字, 附带背景
    /// </summary>
    /// <param name="hdc"></param>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="digit"></param>
    void DrawBkDigit(HDC hdc, int x, int y, int digit) {
        if ( digit < 0 || digit > 9 ) return;

        const auto& segments = DigitSegments[ digit ];

        constexpr int w = SEG_WIDTH;
        constexpr int l = SEG_LENGTH;

        const auto draw_segent = [&](int seg_x, int seg_y, int width, int height, BOOL has) -> void {
            drawSegment(hdc, seg_x, seg_y, width, height, !has);
            };

        const auto has_top = std::find(segments.begin(), segments.end(), SI::TOP) != segments.end();
        const auto has_bottom = std::find(segments.begin(), segments.end(), SI::BOTTOM) != segments.end();
        const auto has_middle = std::find(segments.begin(), segments.end(), SI::MIDDLE) != segments.end();
        const auto has_left_up = std::find(segments.begin(), segments.end(), SI::LEFT_UP) != segments.end();
        const auto has_right_up = std::find(segments.begin(), segments.end(), SI::RIGHT_UP) != segments.end();
        const auto has_left_down = std::find(segments.begin(), segments.end(), SI::LEFT_DOWN) != segments.end();
        const auto has_right_down = std::find(segments.begin(), segments.end(), SI::RIGHT_DOWN) != segments.end();

        draw_segent(x + w, y, l, w, has_top);
        draw_segent(x + w, y + 2 * ( l + w ), l, w, has_bottom);
        draw_segent(x + w, y + ( l + w ), l, w, has_middle);
        draw_segent(x, y + w, w, l, has_left_up);
        draw_segent(x, y + l + 2 * w, w, l, has_left_down);
        draw_segent(x + w + l, y + w, w, l, has_right_up);
        draw_segent(x + w + l, y + l + 2 * w, w, l, has_right_down);
    }

    // 绘制时钟
    void DrawClock(HDC hdc, std::wstring& time_str) {
        if ( time_str.empty() ) return;

        int start_x = PAINT_START_X;
        int start_y = PAINT_START_Y;

        for ( wchar_t c : time_str ) {
            // 数字
            if ( c >= L'0' && c <= L'9' ) {
                int digit = c - L'0';
                DrawBkDigit(hdc, start_x, start_y, digit);
                // 每个数码管数字的长度是 SEG_LENGTH + 2*SEG_WIDTH
                start_x += SEG_LENGTH + 2 * SEG_WIDTH + DIGIT_SPACING;
            }

            // 冒号
            if ( c == L':' ) {
                int colon_y = start_y + ( SEG_LENGTH * 2 + SEG_WIDTH * 3 ) / 2;

                IGDI::AutoGDI<HBRUSH> brush(CreateSolidBrush(RGB(0, 0, 0)));
                HBRUSH old_brash = ( HBRUSH ) SelectObject(hdc, brush);

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
