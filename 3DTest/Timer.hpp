#pragma once
#include <chrono>

namespace Debug {
	enum  TimeType { kNano = 0, kMicro, kMilli, kSecond };
	class Timer { 
	protected:
		std::chrono::time_point<std::chrono::steady_clock> startTime;
		static std::string _timeName[];
		std::string name;
		TimeType timeType;
	public:
		Timer(std::string _name = "Timer", TimeType _timeType = TimeType::kMilli);
		void Stop();
	};

	class ScopedTimer : Timer{
	public:
		ScopedTimer(std::string _name = "Timer", TimeType _timeType = TimeType::kMilli) : Timer(_name, _timeType) { };
		~ScopedTimer();
	};
}

