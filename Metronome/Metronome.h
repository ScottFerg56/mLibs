/*
 Name:		Metronome.h
 Created:	1/27/2018 9:22:29 PM
 Author:	Scott Ferguson
 Editor:	http://www.visualmicro.com
*/

#ifndef _Metronome_h
#define _Metronome_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Metronome
{
public:
	uint32_t	PeriodMS;

	Metronome(uint32_t periodMS) { PeriodMS = periodMS; LastTime = millis(); }

	bool Test();
	void Restart() { LastTime = millis(); }

	operator bool() { return Test(); }

private:
	uint32_t	LastTime;
};

class Micronome
{
public:
	uint32_t	PeriodMicroS;

	Micronome(uint32_t periodMicroS) { PeriodMicroS = periodMicroS; LastTime = micros(); }

	bool Test();
	void Restart() { LastTime = micros(); }

	operator bool() { return Test(); }

private:
	uint32_t	LastTime;
};

#endif
