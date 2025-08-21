#pragma once
#include <ctime>
#include <sstream>
#include <chrono>

namespace Date {

	std::wstring CurrTimeWStr();

	int CurrTimeHour();
	int CurrTimeMin();
	int CurrTimeSec();
}