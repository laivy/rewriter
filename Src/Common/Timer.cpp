#include "Stdafx.h"
#include "Timer.h"

Timer::Timer() :
	m_lastTimePoint{ std::chrono::steady_clock::now() }
{
}

float Timer::Tick()
{
	auto now{ std::chrono::steady_clock::now() };
	auto last{ m_lastTimePoint };
	m_lastTimePoint = now;

	std::chrono::duration<float> deltaTime{ now - last };
	return deltaTime.count();
}
