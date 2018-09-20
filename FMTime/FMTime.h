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

 Author:	Scott Ferguson
*/

#ifndef _FMTime_h
#define _FMTime_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/// <summary>Represents an instant in time, typically expressed as a date and time of day.</summary>
/// <remarks>
/// FMDateTime provides a representation for an instant in time that includes a date and a time
/// which includes milliseconds.
/// A Now() function is provided which returns the current datetime.
/// The time can be set from some external source, with accuracy over time depending upon the processor,
/// but should probably be resfreshed on at least a daily basis for best results.
/// If the time is not set, the Now() time will appear as if the processor restarted at midnight 1/1/1970.
/// </remarks>
class FMDateTime
{
public:
	uint16_t	MilliSecond;	// Millisecond 0 to 999
	uint8_t		Second;			// Second 0 to 59
	uint8_t		Minute;			// Minute 0 to 59
	uint8_t		Hour;			// Hour 0 to 23
	uint8_t		Day;			// Day 1 to 28/29/30/32
	uint8_t		Month;			// Month 1 to 12
	uint16_t	Year;			// Actual year (NOT offset from 1970)

	FMDateTime(const char*s);
	FMDateTime(int64_t ms);
	int64_t		ToMillis();
	uint8_t		WeekDay();
	String		ToString();

	/// <summary>Returns the current datetime.</summary>
	static FMDateTime	Now() { return FMDateTime(NowMillis()); }
	static int64_t		NowMillis();
	static void			SetTime(int64_t ms);
	/// <summary>Sets the current datetime from another datetime.</summary>
	static void			SetTime(FMDateTime dt) { SetTime(dt.ToMillis()); }
	static uint8_t		DaysInMonth(uint8_t month, uint16_t year);
	/// <summary>Returns the number of days in the specified year.</summary>
	static uint16_t		DaysInYear(uint16_t year) { return IsLeapYear(year) ? 366 : 365; }
	/// <summary>Returns true if the specified year is a leap year.</summary>
	static bool			IsLeapYear(uint16_t year) { return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); }
	uint32_t			FullDaysSince1970();

private:
	static int64_t		SystemTime;		// The current time as the number of milliseconds since 1/1/1970 as of the LastUpdate
	static uint32_t		LastUpdate;		// The processor time in milliseconds when the SystemTime was last updated
	static String		DigitsStr(uint32_t digits, byte width);
};


#endif
