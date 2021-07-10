#include "Engine/Utilities/Timer.h"

rv::Timer::Timer()
	:
	last(now())
{
}

float rv::Timer::Peek() const
{
	return std::chrono::duration<float>(now() - last).count();
}

float rv::Timer::Mark()
{
	auto temp = last;
	return std::chrono::duration<float>((last = now()) - temp).count();
}

void rv::Timer::Reset()
{
	last = now();
}

std::chrono::steady_clock::time_point rv::Timer::now()
{
	return std::chrono::steady_clock::now();
}
