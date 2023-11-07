#include "Stdafx.h"
#include "Timer.h"

Timer::Timer() : 
	m_lastClockCount{},
	m_deltaTime{}
{
	QueryPerformanceFrequency(&m_frequency);
}

void Timer::Tick()
{
	LARGE_INTEGER currClockCount;
	QueryPerformanceCounter(&currClockCount);
	m_deltaTime = (currClockCount.QuadPart - m_lastClockCount.QuadPart) / static_cast<float>(m_frequency.QuadPart);
	m_lastClockCount = currClockCount;
}

float Timer::GetDeltaTime() const
{
	return m_deltaTime;
}
