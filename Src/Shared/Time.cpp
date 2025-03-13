#include "Time.h"

Time::Time(int year, int month, int day, int hour, int min, int sec) :
	m_ymd{ static_cast<std::chrono::year>(year), static_cast<std::chrono::month>(month), static_cast<std::chrono::day>(day) },
	m_hms{ std::chrono::hours{ hour } + std::chrono::minutes{ min } + std::chrono::seconds{ sec } }
{
	auto days{ std::chrono::local_days{ m_ymd }.time_since_epoch() };
	m_time = std::chrono::local_time{ days + m_hms.to_duration() };
}

Time Time::Now()
{
	auto now{ std::chrono::current_zone()->to_local(std::chrono::system_clock::now()) };
	auto days{ std::chrono::floor<std::chrono::days>(now) };
	std::chrono::year_month_day ymd{ days };
	std::chrono::hh_mm_ss hms{ now - days };

	Time time{};
	time.m_time = now;
	time.m_ymd = ymd;
	time.m_hms = hms;
	return time;
}

bool Time::operator<(const Time& rhs)
{
	return m_time < rhs.m_time;
}

bool Time::operator<=(const Time& rhs)
{
	return m_time <= rhs.m_time;
}

bool Time::operator>(const Time& rhs)
{
	return m_time > rhs.m_time;
}

bool Time::operator>=(const Time& rhs)
{
	return m_time >= rhs.m_time;
}

bool Time::operator==(const Time& rhs)
{
	return m_time == rhs.m_time;
}

int Time::Year() const
{
	return static_cast<int>(m_ymd.year());
}

int Time::Month() const
{
	return static_cast<int>(static_cast<unsigned int>(m_ymd.month()));
}

int Time::Day() const
{
	return static_cast<int>(static_cast<unsigned int>(m_ymd.day()));
}

int Time::Hour() const
{
	return m_hms.hours().count();
}

int Time::Min() const
{
	return m_hms.minutes().count();
}

int Time::Sec() const
{
	return static_cast<int>(m_hms.seconds().count());
}

void Time::CalcDateTime()
{
	auto days{ std::chrono::floor<std::chrono::days>(m_time) };
	m_ymd = std::chrono::year_month_day{ days };
	m_hms = std::chrono::hh_mm_ss{ m_time - days };
}
