#include "Stdafx.h"
#include "Timer.h"

Timer::Timer() :
	m_lastClockCount{},
	m_frequency{},
	m_deltaTime{}
{
	LARGE_INTEGER freq{};
	::QueryPerformanceFrequency(&freq);
	m_frequency = 1.0f / freq.QuadPart;
}

float Timer::Tick()
{
	LARGE_INTEGER currClockCount{};
	::QueryPerformanceCounter(&currClockCount);
	m_deltaTime = (currClockCount.QuadPart - m_lastClockCount.QuadPart) * m_frequency;
	m_lastClockCount = currClockCount;
	return m_deltaTime;
}
