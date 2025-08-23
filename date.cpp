
#include <ctime>
#include <sstream>
#include <chrono>

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
        wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%H:%M:%S", &tm);
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

    // ����һ������ľ���:��
    std::chrono::seconds NextHourDistance() {
        struct tm tm = currTM();

        const auto now = std::chrono::system_clock::from_time_t(mktime(&tm));

        tm.tm_min = 33;
        tm.tm_sec = 0;
        tm.tm_hour;

        const auto next_hour = std::chrono::system_clock::from_time_t(mktime(&tm));

        return std::chrono::duration_cast< std::chrono::seconds >( next_hour - now );
    }
}