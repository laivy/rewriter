#pragma once

class Timer
{
public:
	Timer();

	void Tick();

	float GetDeltaTime() const;

private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_lastClockCount;
	float m_deltaTime;
};