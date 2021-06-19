#pragma once
#include <chrono>

namespace rv
{
	class Timer
	{
	public:
		Timer();

		float Peek() const;
		float Mark();
		void Reset();

	private:
		static std::chrono::steady_clock::time_point now();
		std::chrono::steady_clock::time_point last;
	};
}