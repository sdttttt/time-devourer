
#include <ctime>
#include <sstream>
#include <chrono>

namespace Date {

	namespace {
		struct tm CurrTM() {
			auto now = std::chrono::system_clock::now();
			auto now_t = std::chrono::system_clock::to_time_t(now);

			struct tm tm;
			localtime_s(&tm, &now_t);
			return tm;
		}
	}

	std::wstring CurrTimeWStr() {
		auto tm = CurrTM();

		wchar_t buffer[80];
		wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%H:%M:%S", &tm);
		return buffer;
	}

	int CurrTimeHour() {
		auto tm = CurrTM();
		
		return tm.tm_hour;
	}

	int CurrTimeMin() {
		auto tm = CurrTM();
		return tm.tm_min;
	}

	int CurrTimeSec() {
		auto tm = CurrTM();
		return tm.tm_sec;
	}
}