/*

@@@@@@@ @@   @@  @@@@@@    @@
 @@  @@ @@@ @@@  @ @@ @    @@
 @@   @ @@@@@@@  @ @@ @
 @@ @   @@@@@@@    @@     @@@   @@@ @@   @@@@@
 @@@@   @@ @ @@    @@      @@   @@@@@@@ @@   @@
 @@ @   @@   @@    @@      @@   @@ @ @@ @@@@@@@
 @@     @@   @@    @@      @@   @@ @ @@ @@
 @@     @@   @@    @@      @@   @@ @ @@ @@   @@
@@@@    @@   @@   @@@@    @@@@  @@   @@  @@@@@

	(c) 2018 Scott Ferguson
	This code is licensed under MIT license (see LICENSE file for details)
*/

#include "FMTime.h"
#include <FMDebug.h>

int64_t FMDateTime::SystemTime;
uint32_t FMDateTime::LastUpdate;

/// <summary>Returns the number of days in a specified month and year.</summary>
/// <param name="month">The month (1 to 12).</param>
/// <param name="year">The year (0-based).</param>
/// <returns>The number of days in the month.</returns>
uint8_t FMDateTime::DaysInMonth(uint8_t month, uint16_t year)
{
	// API starts months from 1, this array starts from 0
	static const uint8_t monthDays[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

	uint8_t days = monthDays[month - 1];
	if (month == 2 && IsLeapYear(year))		// february
		days++;
	return days;
}

/// <summary>Construct an FMDateTime from a string.</summary>
/// <param name="s">The datetime in a specific string format.</param>
/// <remarks>
/// The input string must be exactly in the fixed format: "yyyy/MM/dd HH:mm:ss.FFF".
/// </remarks>
FMDateTime::FMDateTime(const char*s)
{
	if (strlen(s) != 23)
		return;
	char ts[24];
	strcpy(ts, s);
	//           1         2
	// 01234567890123456789012
	// yyyy/MM/dd HH:mm:ss.FFF
	// 2018/02/23 14:13:34.741
	if (ts[4] != '/')
		return;
	ts[4] = 0;
	Year = atoi(ts);
	if (ts[7] != '/')
		return;
	ts[7] = 0;
	Month = atoi(ts + 5);
	if (ts[10] != ' ')
		return;
	ts[10] = 0;
	Day = atoi(ts + 8);
	if (ts[13] != ':')
		return;
	ts[13] = 0;
	Hour = atoi(ts + 11);
	if (ts[16] != ':')
		return;
	ts[16] = 0;
	Minute = atoi(ts + 14);
	if (ts[19] != '.')
		return;
	ts[19] = 0;
	Second = atoi(ts + 17);
	MilliSecond = atoi(ts + 20);
}

/// <summary>Construct an FMDateTime from a number of milliseconds since 1/1/1970.</summary>
/// <param name="ms">The number of milliseconds since 1/1/1970.</param>
FMDateTime::FMDateTime(int64_t ms)
{
	MilliSecond = ms % 1000;
	uint32_t time = ms / 1000;	// now it is seconds
	Second = time % 60;
	time /= 60;					// now it is minutes
	Minute = time % 60;
	time /= 60;					// now it is hours
	Hour = time % 24;
	time /= 24;					// now it is days

	Year = 1970;				// the year base is 1970
	while (time > 0)
	{
		int days = DaysInYear(Year);
		if (time < days)
			break;
		time -= days;
		Year++;
	}
	// time is now days in this year, starting at 0

	Month = 1;
	while (time > 0)
	{
		uint8_t days = DaysInMonth(Month, Year);
		if (time < days)
			break;
		time -= days;
		Month++;
	}
	// time is now days in this month, starting at 0
	Day = time + 1;     // day of month
}

/// <summary>Returns the number of days since 1/1/1970.</summary>
/// <returns>The number of days since 1/1/1970.</returns>
uint32_t FMDateTime::FullDaysSince1970()
{
	uint32_t days = 0;
	for (int i = 1970; i < Year; i++)
	{
		days += DaysInYear(i);
	}

	// add days for this year, months start from 1
	for (int i = 1; i < Month; i++)
	{
		days += DaysInMonth(i, Year);
	}

	return days + Day - 1;
}

/// <summary>Returns the number of milliseconds since 1/1/1970.</summary>
/// <returns>The number of milliseconds since 1/1/1970.</returns>
int64_t FMDateTime::ToMillis()
{
	uint32_t seconds = ((FullDaysSince1970() * 24 + Hour) * 60 + Minute) * 60 + Second;
	return (int64_t)seconds * 1000 + MilliSecond;
}

/// <summary>Returns the weekday.</summary>
/// <returns>The weekday, where 1 is Sunday, 2 is Monday, etc.</returns>
uint8_t FMDateTime::WeekDay()
{
	return ((FullDaysSince1970() + 4) % 7) + 1;  // Sunday is day 1 
}

/// <summary>Returns the current time as the number of milliseconds since 1/1/1970.</summary>
/// <returns>The current time as the number of milliseconds since 1/1/1970.</returns>
int64_t FMDateTime::NowMillis()
{
	// The current time at any instant is the SystemTime plus time elapsed since the LastUpdate
	// Update the SystemTime every call here because the system millis() value will overflow
	// about every 50 days if the time is not set sooner.
	uint32_t ms = millis();
	SystemTime += ms - LastUpdate;
	LastUpdate = ms;
	return SystemTime;
}

/// <summary>Convert a value into a fixed-width string.</summary>
/// <param name="digits">The value to convert.</param>
/// <param name="width">The desired width.</param>
/// <returns>The String representing the value, left-filled with 0's to the specified width.</returns>
String FMDateTime::DigitsStr(uint32_t digits, byte width)
{
	String s = String(digits);
	while (s.length() < width)
		s = "0" + s;
	return s;
}

/// <summary>Returns a formatted string representing the datetime.</summary>
/// <returns>The string representing the datetime with the fixed format: "yyyy/MM/dd HH:mm:ss.FFF".</returns>
String FMDateTime::ToString()
{
	return String(Year) +
		"/" + DigitsStr(Month, 2) +
		"/" + DigitsStr(Day, 2) +
		" " + DigitsStr(Hour, 2) +
		":" + DigitsStr(Minute, 2) +
		":" + DigitsStr(Second, 2) +
		"." + DigitsStr(MilliSecond, 3);
}

/// <summary>Set the current time with the number of milliseconds since 1/1/1970.</summary>
/// <param name="ms">The current time as the number of milliseconds since 1/1/1970.</param>
void FMDateTime::SetTime(int64_t ms)
{
	SystemTime = ms;
	LastUpdate = millis();
}
