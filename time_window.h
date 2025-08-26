#pragma once

#include <windows.h>
#include <chrono>

// 时间显示窗口类
constexpr auto TIMER_CLASS_NAME = L"TimeWindowClass";
// 标题栏名
constexpr auto TIMER_WINDOW_TITLE = L"Timer";

// 窗口尺寸
constexpr auto TIMER_WINDOW_WIDTH = 320;
constexpr auto TIMER_WINDOW_HEIGHT = 100;
// 窗口坐标
constexpr auto TIMER_WINDOW_X = 10;
constexpr auto TIMER_WINDOW_Y = 10;

// 窗口椭圆坐标
constexpr auto TIMER_WINDOW_ROUND_X = 0;
constexpr auto TIMER_WINDOW_ROUND_Y = 0;
// 窗口椭圆的宽度和高度
constexpr auto TIMER_WINDOW_ROUND_W = 10;
constexpr auto TIMER_WINDOW_ROUND_H = 10;

// 窗口的元素间距
constexpr auto TIMER_WINDOW_MARGIN = 10;

// 时间显示容器的圆角宽度和高度
constexpr auto TIMER_DISPLAY_ROUND_W = 10;
constexpr auto TIMER_DISPLAY_ROUND_H = 10;

// 时间的更新定时器
constexpr auto DATE_TIMER_ID = 0x0001;
// 动画的更新定时器
constexpr auto ANIMATION_TIMER_ID = 0x0002;

// 窗口显示时间半径: 秒
constexpr auto WINDOWS_SHOW_TIME_RADIUS_SEC = 30;

// 动画的淡入淡速度
constexpr auto FADE_DURATION = 5;

int CreateTimeClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow);


// 计步器: 并非时间倒计时，由手动调用step步进来计数
// 需要设置起点和目标点，更具distance来计算和目标点的距离
// 抵达目标点后不会停止，可以继续调用step来继续步进
class StepCountDown {

public:
    StepCountDown(long long down, long long count = 0) : count(count), down(down) {}
    StepCountDown(std::chrono::seconds down, std::chrono::seconds count = std::chrono::seconds(0)) : count(count.count()), down(down.count()) {}

    StepCountDown() : StepCountDown(0, 0) {};

    void step(UINT i = 1) {
        this->count += i;
    }

    long long distance() const {
        return abs(static_cast< int >( this->down - this->count ));
    }

    void reset(long long end, long long start = 0) {
        this->count = start;
        this->down = end;
    }

    void reset(std::chrono::seconds end, long long start = 0) {
        this->count = start;
        this->down = end.count();
    }

private:

    long long count;
    long long down;
};
