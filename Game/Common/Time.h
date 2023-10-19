#pragma once
#include <chrono>

template <class T>
concept IsChronoDuration = requires
{
	std::is_same_v<T, std::chrono::year>;
	std::is_same_v<T, std::chrono::years>;
	std::is_same_v<T, std::chrono::month>;
	std::is_same_v<T, std::chrono::months>;
	std::is_same_v<T, std::chrono::day>;
	std::is_same_v<T, std::chrono::days>;
	std::is_same_v<T, std::chrono::hours>;
	std::is_same_v<T, std::chrono::minutes>;
	std::is_same_v<T, std::chrono::seconds>;
};

class Time
{
public:
	Time();

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
		if constexpr (std::is_same_v<T, std::chrono::year>)
		{
			std::chrono::years years{ static_cast<int>(duration) };
			return *this + years;
		}
		else if constexpr (std::is_same_v<T, std::chrono::month>)
		{
			std::chrono::months months{ static_cast<unsigned int>(duration) };
			return *this + months;
		}
		else if constexpr (std::is_same_v<T, std::chrono::day>)
		{
			std::chrono::days days{ static_cast<unsigned int>(duration) };
			return *this + days;
		}
		else
		{
			return Time{ m_time + std::chrono::duration_cast<std::chrono::seconds>(duration).count() };
		}
	}

	//template <class T>
	//requires IsChronoYMD<T> || IsChronoHMS<T>
	//Time& operator+=(const T& duration)
	//{
	//	m_time += std::chrono::duration_cast<std::chrono::seconds>(duration);
	//	return *this;
	//}

	int Year() const;
	unsigned int Month() const;
	unsigned int Day() const;
	int Hour() const;
	int Min() const;
	int Sec() const;
	int Milli() const;
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