#pragma once
#include <chrono>

template <class T>
concept is_time_duration = 
	std::is_same_v<T, std::chrono::day> ||
	std::is_same_v<T, std::chrono::days> ||
	std::is_same_v<T, std::chrono::hours> ||
	std::is_same_v<T, std::chrono::minutes> ||
	std::is_same_v<T, std::chrono::seconds>;

class Time
{
public:
	Time(int year = 0, int month = 0, int day = 0, int hour = 0, int min = 0, int sec = 0);

	static Time Now();

	bool operator<(const Time& rhs);
	bool operator<=(const Time& rhs);
	bool operator>(const Time& rhs);
	bool operator>=(const Time& rhs);
	bool operator==(const Time& rhs);

	template <class T>
	requires is_time_duration<T>
	Time operator+(const T& duration)
	{
		Time time{};
		time.m_time = m_time + duration;
		time.CalcDateTime();
		return time;
	}

	template <class T>
	requires is_time_duration<T>
	Time operator-(const T& duration)
	{
		Time time{};
		time.m_time = m_time - duration;
		time.CalcDateTime();
		return time;
	}

	template <class T>
	requires is_time_duration<T>
	Time& operator+=(const T& duration)
	{
		m_time += duration;
		CalcDateTime();
		return *this;
	}

	template <class T>
	requires is_time_duration<T>
	Time& operator-=(const T& duration)
	{
		m_time -= duration;
		CalcDateTime();
		return *this;
	}

	int Year() const;
	int Month() const;
	int Day() const;
	int Hour() const;
	int Min() const;
	int Sec() const;

private:
	void CalcDateTime();

private:
	std::chrono::local_time<std::chrono::system_clock::duration> m_time;
	std::chrono::year_month_day m_ymd;
	std::chrono::hh_mm_ss<std::chrono::system_clock::duration> m_hms;
};
