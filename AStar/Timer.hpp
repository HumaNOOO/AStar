#pragma once

#include <functional>
#include <chrono>


namespace astar
{
	class Timer
	{
	public:
		Timer(std::function<void()> callback = std::function<void()>{}, long long delay = 0);
		~Timer();
	private:
		std::function<void()> callback_;
		long long delay_;
		std::chrono::time_point<std::chrono::steady_clock> start_;
	};
}
