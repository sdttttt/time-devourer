#pragma once

#include <windows.h>
#include <chrono>

// ʱ����ʾ������
constexpr const TCHAR* TIMER_CLASS_NAME = L"TimeWindowClass";
// ��������
constexpr const TCHAR* TIMER_WINDOW_TITLE = L"Timer";

// ���ڳߴ�
constexpr UINT TIMER_WINDOW_WIDTH = 320;
constexpr UINT TIMER_WINDOW_HEIGHT = 100;
// ��������
constexpr UINT TIMER_WINDOW_X = 10;
constexpr UINT TIMER_WINDOW_Y = 10;

// ������Բ����
constexpr UINT TIMER_WINDOW_ROUND_X = 0;
constexpr UINT TIMER_WINDOW_ROUND_Y = 0;
// ������Բ�Ŀ�Ⱥ͸߶�
constexpr UINT TIMER_WINDOW_ROUND_W = 10;
constexpr UINT TIMER_WINDOW_ROUND_H = 10;

// ���ڵ�Ԫ�ؼ��
constexpr UINT TIMER_WINDOW_MARGIN = 10;

// ʱ����ʾ������Բ�ǿ�Ⱥ͸߶�
constexpr UINT TIMER_DISPLAY_ROUND_W = 10;
constexpr UINT TIMER_DISPLAY_ROUND_H = 10;

// ʱ��ĸ��¶�ʱ��
constexpr UINT_PTR DATE_TIMER_ID = 0x0001;
// �����ĸ��¶�ʱ��
constexpr UINT_PTR ANIMATION_TIMER_ID = 0x0002;

// ������ʾʱ��뾶: ��
constexpr UINT WINDOWS_SHOW_TIME_RADIUS_SEC = 30;

// �����ĵ��뵭�ٶ�
constexpr UINT FADE_DURATION = 5;

int CreateTimeClassAndWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow);


// �Ʋ���: ����ʱ�䵹��ʱ�����ֶ�����step����������
// ��Ҫ��������Ŀ��㣬����distance�������Ŀ���ľ���
// �ִ�Ŀ���󲻻�ֹͣ�����Լ�������step����������
class StepCountDown {

public:
    StepCountDown(UINT down, UINT count = 0) : count(count), down(down) {}
    StepCountDown(std::chrono::seconds down, std::chrono::seconds count = std::chrono::seconds(0)) : count(count.count()), down(down.count()) {}

    StepCountDown() : StepCountDown(0, 0) {};

    void step(UINT i = 1) {
        this->count += i;
    }

    UINT distance() const {
        return abs(static_cast< int >( this->down - this->count ));
    }

    void reset(UINT end, UINT start = 0) {
        this->count = start;
        this->down = end;
    }

    void reset(std::chrono::seconds end, UINT start = 0) {
        this->count = start;
        this->down = end.count();
    }

private:

    UINT count;
    UINT down;
};
