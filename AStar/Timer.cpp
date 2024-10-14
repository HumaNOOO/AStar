#include "Timer.hpp"


namespace astar
{
	Timer::Timer(std::function<void()> callback, const long long delay) : callback_{callback}, delay_{delay}, start_{std::chrono::steady_clock::now()} {}
}
