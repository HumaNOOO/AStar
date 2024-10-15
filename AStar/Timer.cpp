#include "Timer.hpp"
#include <iostream>


namespace astar
{
	Timer::Timer(std::function<void()> callback, const long long delay) : callback_{callback}, delay_{delay}, start_{std::chrono::steady_clock::now()} {}
	Timer::~Timer() {
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start_).count()<<'\n';

	}

}
