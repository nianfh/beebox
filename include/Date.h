
#ifndef __DATE_H__
#define __DATE_H__

#include <time.h>
#include <string>

namespace beebox
{

typedef struct tm Time;
typedef time_t Seconds;

// use function "toString to format date
/******************************日期格式化说明*****************************
符号    作用                                                       实例
 %a     缩写的星期几名称                                            Sun
 %A     完整的星期几名称                                            Sunday
 %b     缩写的月份名称                                              Mar
 %B     完整的月份名称                                              March
 %c     日期和时间表示法                                            Sun Aug 19 02:56:02 2012
 %d     一月中的第几天（01-31）                                     19
 %H     24 小时格式的小时（00-23）                                  14
 %I     12 小时格式的小时（01-12）                                  05
 %j     一年中的第几天（001-366）                                   231
 %m     十进制数表示的月份（01-12）                                  08
 %M     分（00-59）                                                55
 %p     AM 或 PM 名称                                              PM
 %S     秒（00-61）                                                02
 %U     一年中的第几周，以第一个星期日作为第一周的第一天（00-53）         33
 %w     十进制数表示的星期几，星期日表示为 0（0-6）                     4
 %W     一年中的第几周，以第一个星期一作为第一周的第一天（00-53）         34
 %x     日期表示法                                                 08/19/12
 %X     时间表示法                                                 02:50:06
 %y     年份，最后两个数字（00-99）                                  01
 %Y     年份                                                      2016
 %Z     时区的名称或缩写	                                        CDT
 %%     一个 % 符号                                                %
 **********************************************************************/

class CDate
{
public:
	enum time_type
	{
		UTC,       // UTC, Universal Time Coordinated
		GMT = UTC, // GMT, Greenwich Mean Time, as same as UTC
		LOCAL      // local time
	};

	enum add_unit
	{
		SECOND,
		MINUTE,
		HOUR,
		DAY,
		WEEK,
		MONTH,
		YEAR
	};

	CDate(time_type type=LOCAL);
	CDate(long long seconds);
	CDate(int year, int month, int day, int hour=0, int minute=0, int second=0);
	~CDate();

	int getDate(); // 返回一个月中的某一天 (1 ~ 31)
	int getDay(); // 返回一周中的某一天 (0 ~ 6, 0为sunday)
	int getMonth(); // 返回月份 (1 ~ 12)
	int getYear(); // 以四位数字返回年份
	int getHour(); // 返回小时(0 ~23)
	int getMinute(); // 返回分钟 (0 ~ 59)
	int getSecond(); // 返回秒数 (0 ~ 59)

	void setDate(int day); // 设置一个月中的某一天 (1 ~ 31)
	void setDay(int day); // 设置一周中的某一天 (0 ~ 6, 0为sunday)
	void setMonth(int month); // 设置月份 (1 ~ 12)
	void setYear(int year); // 以四位数字设置年份
	void setHour(int hour); // 设置小时(0 ~23)
	void setMinute(int minute); // 设置分钟 (0 ~ 59)
	void setSecond(int second); // 设置秒数 (0 ~ 59)

	bool operator > (CDate& a);
	bool operator < (CDate& a);
	bool operator == (CDate& a);

	CDate addTime(int addUnit, long long offset);
	bool isLeepYear();
	int getDaysOfMonth(int month = 0);

	long long toSeconds(); // 返回 1970 年 1 月 1 日至今的秒数
	std::string toString();
	std::string toString(const char * format);
	std::string toStringForHttp();

	bool fromString(std::string s);
	static CDate fromDateTimeMacro();

private:
	Time m_time;
};

}

#endif // __DATE_H__
