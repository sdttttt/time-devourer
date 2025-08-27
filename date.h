#pragma once
#include <ctime>
#include <sstream>
#include <chrono>

namespace Date {
    constexpr auto TIME_FORMAT = L"%H:%M:%S";

    constexpr size_t TIME_FORMAT_LEN() {
        auto len = 0;
        while (TIME_FORMAT[len] != L'\0') {
            len++;
        }
        return ++len;
    }

    std::wstring CurrTimeWStr();

    std::chrono::hours CurrTimeHour();

    std::chrono::minutes CurrTimeMin();

    std::chrono::seconds CurrTimeSec();

    std::chrono::seconds NextHourDistance();
}
