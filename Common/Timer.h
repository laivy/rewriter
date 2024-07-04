#pragma once

class Timer
{
public:
	Timer();
	~Timer() = default;

	float Tick();

private:
	LARGE_INTEGER m_lastClockCount;
	float m_frequency;
	float m_deltaTime;
};