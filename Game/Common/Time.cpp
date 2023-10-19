#include "Time.h"

using namespace std::chrono;
using namespace std::chrono_literals;

Time::Time() : m_time{}
{
}

Time Time::Now()
{
	return Time{ system_clock::to_time_t(system_clock::now()) };
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
	return static_cast<int>(GetYMD().year());
}

unsigned int Time::Month() const
{
	return static_cast<unsigned int>(GetYMD().month());
}

unsigned int Time::Day() const
{
	return static_cast<unsigned int>(GetYMD().day());
}

int Time::Hour() const
{
	return GetHMS().hours().count();
}

int Time::Min() const
{
	return GetHMS().minutes().count();
}

int Time::Sec() const
{
	return static_cast<int>(GetHMS().seconds().count());
}

int Time::Milli() const
{
	return static_cast<int>(GetHMS().subseconds().count());
}

std::tuple<int, int, int> Time::YMD() const
{
	return std::make_tuple(Year(), Month(), Day());
}

std::tuple<int, int, int> Time::HMS() const
{
	return std::make_tuple(Hour(), Min(), Sec());
}

Time::Time(std::time_t time) : m_time{ time }
{
}

local_time<system_clock::duration> Time::GetLocalTime() const
{
	return zoned_time{ current_zone(), system_clock::from_time_t(m_time) }.get_local_time();
}

year_month_day Time::GetYMD() const
{
	return year_month_day{ floor<days>(GetLocalTime()) };
}

hh_mm_ss<milliseconds> Time::GetHMS() const
{
	auto localTime{ GetLocalTime() };
	return hh_mm_ss{ floor<milliseconds>(localTime - floor<days>(localTime)) };
}