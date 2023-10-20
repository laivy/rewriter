#pragma once
#include <chrono>

template <class T>
concept IsChronoDuration = 
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
	requires IsChronoDuration<T>
	Time operator+(const T& duration)
	{
		if constexpr (std::is_same_v<T, std::chrono::day>)
		{
			std::chrono::days days{ static_cast<unsigned int>(duration) };
			return *this + days;
		}
		else
		{
			return Time{ m_time + std::chrono::duration_cast<std::chrono::seconds>(duration).count() };
		}
	}

	template <class T>
	requires IsChronoDuration<T>
	Time& operator+=(const T& duration)
	{
		if constexpr (std::is_same_v<T, std::chrono::day>)
		{
			std::chrono::days days{ static_cast<unsigned int>(duration) };
			m_time += std::chrono::duration_cast<std::chrono::seconds>(days).count();
		}
		else
		{
			m_time += std::chrono::duration_cast<std::chrono::seconds>(duration).count();
		}
		return *this;
	}

	template <class T>
	requires IsChronoDuration<T>
	Time operator-(const T& duration)
	{
		if constexpr (std::is_same_v<T, std::chrono::day>)
		{
			std::chrono::days days{ static_cast<unsigned int>(duration) };
			return *this - days;
		}
		else
		{
			return Time{ m_time - std::chrono::duration_cast<std::chrono::seconds>(duration).count() };
		}
	}

	template <class T>
	requires IsChronoDuration<T>
	Time& operator-=(const T& duration)
	{
		if constexpr (std::is_same_v<T, std::chrono::day>)
		{
			std::chrono::days days{ static_cast<unsigned int>(duration) };
			m_time -= std::chrono::duration_cast<std::chrono::seconds>(days).count();
		}
		else
		{
			m_time -= std::chrono::duration_cast<std::chrono::seconds>(duration).count();
		}
		return *this;
	}

	int Year() const;
	unsigned int Month() const;
	unsigned int Day() const;
	int Hour() const;
	int Min() const;
	int Sec() const;
	std::tuple<int, int, int> YMD() const;
	std::tuple<int, int, int> HMS() const;

private:
	Time(std::time_t time);
	std::chrono::local_time<std::chrono::system_clock::duration> GetLocalTime() const;
	std::chrono::year_month_day GetYMD() const;
	std::chrono::hh_mm_ss<std::chrono::milliseconds> GetHMS() const;

private:
	std::time_t m_time;
};