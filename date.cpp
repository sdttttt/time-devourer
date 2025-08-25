
#include <ctime>
#include <sstream>
#include <chrono>
#include "date.h"

namespace Date {

    namespace {
        inline struct tm currTM() {
            const auto now = std::chrono::system_clock::now();
            const auto now_t = std::chrono::system_clock::to_time_t(now);

            struct tm tm;
            localtime_s(&tm, &now_t);
            return tm;
        }
    }

    std::wstring CurrTimeWStr() {
        const auto tm = currTM();

        wchar_t buffer[ 80 ];
        wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), TIME_FORMAT, &tm);
        return buffer;
    }



    std::chrono::hours CurrTimeHour() {
        auto tm = currTM();

        return std::chrono::hours(tm.tm_hour);
    }

    std::chrono::minutes CurrTimeMin() {
        auto tm = currTM();
        return std::chrono::minutes(tm.tm_min);
    }

    std::chrono::seconds CurrTimeSec() {
        auto tm = currTM();
        return std::chrono::seconds(tm.tm_sec);
    }

#ifdef _DEBUG
    std::chrono::seconds NextHourDistance() {
        struct tm tm = currTM();

        const auto now = std::chrono::system_clock::from_time_t(mktime(&tm));

        tm.tm_min+= 2;
        tm.tm_sec = 0;

        const auto next_hour = std::chrono::system_clock::from_time_t(mktime(&tm));

        return std::chrono::duration_cast< std::chrono::seconds >( next_hour - now );
    }
#endif

#ifdef NDEBUG
    // 和下一个整点的距离:秒
    std::chrono::seconds NextHourDistance() {
        struct tm tm = currTM();

        const auto now = std::chrono::system_clock::from_time_t(mktime(&tm));

        tm.tm_hour++;
        tm.tm_min = 0;
        tm.tm_sec = 0;

        const auto next_hour = std::chrono::system_clock::from_time_t(mktime(&tm));

        return std::chrono::duration_cast< std::chrono::seconds >( next_hour - now );
    }
#endif

}