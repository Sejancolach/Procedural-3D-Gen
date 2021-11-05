#include "Timer.hpp"

namespace Debug {

	Timer::Timer(std::string _name, TimeType _timeType) {
		name = _name;
		startTime = std::chrono::high_resolution_clock::now();
		timeType = _timeType;
	}

	void Timer::Stop() { 
		long long _time = 0;
		auto elapsed = std::chrono::high_resolution_clock::now() - startTime;
		switch(timeType) {
		case TimeType::kNano:
			_time = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
			break;
		case TimeType::kMicro:
			_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
			break;
		case TimeType::kMilli:
			_time = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
			break;
		case TimeType::kSecond:
			_time = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
			break;
		}
		printf("Timer %s finished with %I64d %s\n", name.c_str(), _time, _timeName[timeType].c_str());
	}

	ScopedTimer::~ScopedTimer() { 
		Stop();
	}
	std::string Timer::_timeName[] = { "ns","qs","ms","s" };
}
