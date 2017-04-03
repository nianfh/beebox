
#include "Date.h"

#include <stdio.h>
#include <string.h>

namespace beebox
{

CDate::CDate(time_type type)
{
	Seconds s = ::time(NULL);

	if (type == LOCAL)
	{
		m_time = *(localtime(&s));
	}
	else
	{
		m_time = *(gmtime(&s));
	}
}

CDate::CDate(long long seconds)
{
	Seconds s = seconds;
	m_time = *(localtime(&s));
}

CDate::CDate(int year, int month, int day, int hour, int minute, int second)
{
	Seconds rawtime;
	time(&rawtime);
	m_time = *localtime(&rawtime);

	m_time.tm_year = year - 1900;
	m_time.tm_mon = month - 1;
	m_time.tm_mday = day;
	m_time.tm_hour = hour;
	m_time.tm_min = minute;
	m_time.tm_sec = second;

	mktime(&m_time);
}

CDate::~CDate()
{
}

int CDate::getDate()
{
	return m_time.tm_mday;
}

int CDate::getDay()
{
	return m_time.tm_wday;
}

int CDate::getMonth()
{
	return m_time.tm_mon + 1;
}

int CDate::getYear()
{
	return m_time.tm_year + 1900;
}

int CDate::getHour()
{
	return m_time.tm_hour;
}

int CDate::getMinute()
{
	return m_time.tm_min;
}

int CDate::getSecond()
{
	return m_time.tm_sec;
}

void CDate::setDate(int day)
{
	if (day < 1 || day > 31)
	{
		return;
	}

	m_time.tm_mday = day;
	mktime(&m_time);
}

void CDate::setDay(int day)
{
	if (day < 0 || day > 6)
	{
		return;
	}

	int offset = day - m_time.tm_wday;
	*this = addTime(DAY, offset);
}

void CDate::setMonth(int month)
{
	if (month < 1 || month > 12)
	{
		return;
	}

	m_time.tm_mon = month - 1;
	mktime(&m_time);
}

void CDate::setYear(int year)
{
	m_time.tm_year = year - 1900;
	mktime(&m_time);
}

void CDate::setHour(int hour)
{
	if (hour < 0 || hour > 23)
	{
		return;
	}

	m_time.tm_hour = hour;
}

void CDate::setMinute(int minute)
{
	if (minute < 0 || minute > 59)
	{
		return;
	}

	m_time.tm_min = minute;
}

void CDate::setSecond(int second)
{
	if (second < 0 || second > 59)
	{
		return;
	}

	m_time.tm_sec = second;
}

bool CDate::operator<(CDate& a)
{
	return toSeconds() < a.toSeconds();
}

bool CDate::operator>(CDate& a)
{
	return toSeconds() < a.toSeconds();
}

bool CDate::operator==(CDate& a)
{
	return toSeconds() == a.toSeconds();
}

CDate CDate::addTime(int addUnit, long long offset)
{
	switch (addUnit)
	{
	case SECOND:
		return CDate(toSeconds() + offset);
	case MINUTE:
		return CDate(toSeconds() + offset * 60);
	case HOUR:
		return CDate(toSeconds() + offset * 60 * 60);
	case DAY:
		return CDate(toSeconds() + offset * 60 * 60 * 24);
	case WEEK:
		return CDate(toSeconds() + offset * 60 * 60 * 24 * 7);
	case MONTH:
	{
		int month = getMonth() + (int)offset;
		int year = getYear();
		if (month >= 12 || month <= 12)
		{
			year += month / 12;
		}

		month = month % 12;

		return CDate(year, month, getDate(), getHour(), getMinute(), getSecond());
	}
	case YEAR:
	default:
		return CDate(getYear() + (int)offset, getMonth(), getDate(), getHour(), getMinute(), getSecond());
	}
}

bool CDate::isLeepYear()
{
	int year = getYear();
	if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
	{
		return true;
	}
	return false;
}

int CDate::getDaysOfMonth(int month)
{
	if (month < 1 || month > 12)
	{
		month = getMonth();
	}

	int months[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
	if (isLeepYear())
	{
		months[2] = 29;
	}

	return months[month];
}

long long CDate::toSeconds()
{
	return (long long)mktime(&m_time);
}

std::string CDate::toString()
{
	return toString("%Y-%m-%d %H:%M:%S");
}

std::string CDate::toString(const char * format)
{
	char buf[256] = { 0 };
	strftime(buf, 256, format, &m_time);

	return std::string(buf);
}

std::string CDate::toStringForHttp()
{
	return toString("%a, %d %b %Y %H:%M:%S GMT");
}

bool CDate::fromString(std::string s)
{
	bool isOk = false;
	int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;

	size_t pos = s.find(' ');
	if (pos != std::string::npos)
	{
		int ret = sscanf(s.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
		if (ret == 6)
		{
			Seconds rawtime;
			time(&rawtime);
			m_time = *localtime(&rawtime);

			m_time.tm_year = year - 1900;
			m_time.tm_mon = month - 1;
			m_time.tm_mday = day;
			m_time.tm_hour = hour;
			m_time.tm_min = minute;
			m_time.tm_sec = second;

			mktime(&m_time);

			isOk = true;
		}
	}
	else
	{
		int ret = sscanf(s.c_str(), "%d-%d-%d", &year, &month, &day);
		if (ret == 3)
		{
			Seconds rawtime;
			time(&rawtime);
			m_time = *localtime(&rawtime);

			m_time.tm_year = year - 1900;
			m_time.tm_mon = month - 1;
			m_time.tm_mday = day;
			m_time.tm_hour = 0;
			m_time.tm_min = 0;
			m_time.tm_sec = 0;

			mktime(&m_time);

			isOk = true;
		}
	}

	return isOk;
}

CDate CDate::fromDateTimeMacro()
{
	char s_month[5];

	int month, day, year, hour, minute, second;

	const char monthNams[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

	sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
	sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second);

	month = (strstr(monthNams, s_month) - monthNams) / 3;

	return CDate(year, month+1, day, hour, minute, second);
}

} // namespace beebox
