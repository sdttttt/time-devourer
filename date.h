#pragma once
#include <ctime>
#include <sstream>
#include <chrono>

namespace Date {

    std::wstring CurrTimeWStr();

    std::chrono::hours CurrTimeHour();
    std::chrono::minutes CurrTimeMin();
    std::chrono::seconds CurrTimeSec();

    std::chrono::seconds NextHourDistance();
}