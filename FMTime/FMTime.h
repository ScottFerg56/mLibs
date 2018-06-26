/*
 Name:		FMTime.h
 Created:	2/24/2018 4:33:25 PM
 Author:	Scott Ferguson
 Editor:	http://www.visualmicro.com
*/

#ifndef _FMTime_h
#define _FMTime_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class FMDateTime
{
public:
	uint16_t	MilliSecond;
	uint8_t		Second;
	uint8_t		Minute;
	uint8_t		Hour;
	uint8_t		Day;		// days start at 1
	uint8_t		Month;		// months start at 1
	uint16_t	Year;		// actual year (NOT offset from 1970)

	FMDateTime(const char*s);
	FMDateTime(int64_t ms);
	int64_t		ToMillis();
	uint8_t		WeekDay();
	String		ToString();

	static FMDateTime	Now() { return FMDateTime(NowMillis()); }
	static int64_t		NowMillis();
	static void			SetTime(int64_t ms);
	static void			SetTime(FMDateTime dt) { SetTime(dt.ToMillis()); }
	static uint8_t		DaysInMonth(uint8_t month, uint16_t year);
	static uint16_t		DaysInYear(uint16_t year) { return IsLeapYear(year) ? 366 : 365; }
	static bool			IsLeapYear(uint16_t year) { return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); }
	uint32_t			FullDaysSince1970();

private:
	static int64_t		SystemTime;
	static uint32_t		LastUpdate;
	static String		DigitsStr(uint32_t digits, byte width);
};


#endif

