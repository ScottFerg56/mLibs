/*

@@   @@            @
@@@ @@@           @@
@@@@@@@           @@
@@@@@@@  @@@@@  @@@@@@  @@ @@@   @@@@@  @@ @@@   @@@@@  @@@ @@   @@@@@
@@ @ @@ @@   @@   @@     @@  @@ @@   @@  @@@@@@ @@   @@ @@@@@@@ @@   @@
@@   @@ @@@@@@@   @@     @@  @@ @@   @@  @@  @@ @@   @@ @@ @ @@ @@@@@@@
@@   @@ @@        @@     @@     @@   @@  @@  @@ @@   @@ @@ @ @@ @@
@@   @@ @@   @@   @@ @@  @@     @@   @@  @@  @@ @@   @@ @@ @ @@ @@   @@
@@   @@  @@@@@     @@@  @@@@     @@@@@   @@  @@  @@@@@  @@   @@  @@@@@

	(c) 2018 Scott Ferguson
	This code is licensed under MIT license (see LICENSE file for details)
*/

#ifndef _Metronome_h
#define _Metronome_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/// <summary>A timer implementation with millisecond resolution.</summary>
/// <remarks>
/// Metronome provides a timer implementation that can be polled to space events out
/// with start times at regular intervals.
/// The capacity of the system milliseconds timer, millis(), allows intervals of
/// just under 50 days to be specified.
/// The resolution of the timer is processor-specific and accuracy will vary
/// based on how frequently the timer can be tested.
/// </remarks>
class Metronome
{
public:
	/// <summary>The interval period in milliseconds.</summary>
	uint32_t	PeriodMS;

	/// <summary>Construct with a specified interval in milliseconds.</summary>
	Metronome(uint32_t periodMS) { PeriodMS = periodMS; LastTime = millis(); }

	bool Test();
	/// <summary>Restart the timer interval.</summary>
	void Restart() { LastTime = millis(); }

	/// <summary>Using the object as a boolean expression tests for expiration of the timer interval.</summary>
	operator bool() { return Test(); }

private:
	uint32_t	LastTime;	// the system time, in milliseconds, of the start of the last interval
};

/// <summary>A timer implementation with microsecond resolution.</summary>
/// <remarks>
/// Micronome provides a timer implementation that can be polled to space events out
/// with start times at regular intervals.
/// The capacity of the system milliseconds timer, millis(), allows intervals of
/// just over 71 minutes to be specified.
/// The resolution of the timer is processor-specific and accuracy will vary
/// based on how frequently the timer can be tested.
/// </remarks>
class Micronome
{
public:
	/// <summary>The interval period in microseconds.</summary>
	uint32_t	PeriodMicroS;

	/// <summary>Construct with a specified interval in microseconds.</summary>
	Micronome(uint32_t periodMicroS) { PeriodMicroS = periodMicroS; LastTime = micros(); }

	bool Test();
	/// <summary>Restart the timer interval.</summary>
	void Restart() { LastTime = micros(); }

	/// <summary>Using the object as a boolean expression tests for expiration of the timer interval.</summary>
	operator bool() { return Test(); }

private:
	uint32_t	LastTime;	// the system time, in microseconds, of the start of the last interval
};

#endif
