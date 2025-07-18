export module rewriter.common.timer;

import std;

export class Timer
{
public:
	Timer();
	~Timer() = default;

	float Tick();

private:
	std::chrono::steady_clock::time_point m_lastTimePoint;
};
