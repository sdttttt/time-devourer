#include <ctime>
#include <sstream>
#include <chrono>

namespace Date {

	std::wstring Curr() {
		auto now = std::chrono::system_clock::now();
		auto now_t = std::chrono::system_clock::to_time_t(now);

		struct tm tm;

		localtime_s(&tm, &now_t);

		wchar_t buffer[80];
		wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%H:%M:%S", &tm);
		return buffer;
	}

}