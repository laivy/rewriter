#pragma once

class Timer
{
public:
	Timer();

	void Tick();

	FLOAT GetDeltaTime() const;

private:
	LARGE_INTEGER	m_frequency;
	LARGE_INTEGER	m_lastClockCount;
	FLOAT			m_deltaTime;
};