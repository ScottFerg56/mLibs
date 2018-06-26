/*
 Name:		Metronome.cpp
 Created:	1/27/2018 9:22:29 PM
 Author:	Scott Ferguson
 Editor:	http://www.visualmicro.com
*/

#include "Metronome.h"

bool Metronome::Test()
{
	uint32_t t = millis();
	if (t - LastTime <= PeriodMS)
		return false;
	LastTime = t;
	return true;
}

bool Micronome::Test()
{
	uint32_t t = micros();
	if (t - LastTime <= PeriodMicroS)
		return false;
	LastTime = t;
	return true;
}
