#pragma once
#include <ctime>
#include <sstream>
#include <chrono>

namespace Date {

    constexpr const wchar_t* TIME_FORMAT = L"%H:%M:%S";

    std::wstring CurrTimeWStr();

    std::chrono::hours CurrTimeHour();
    std::chrono::minutes CurrTimeMin();
    std::chrono::seconds CurrTimeSec();

    std::chrono::seconds NextHourDistance();
}