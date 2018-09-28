/*

OO   OO            O
OOO OOO           OO
OOOOOOO           OO
OOOOOOO  OOOOO  OOOOOO  OO OOO   OOOOO  OO OOO   OOOOO  OOO OO   OOOOO
OO O OO OO   OO   OO     OO  OO OO   OO  OOOOOO OO   OO OOOOOOO OO   OO
OO   OO OOOOOOO   OO     OO  OO OO   OO  OO  OO OO   OO OO O OO OOOOOOO
OO   OO OO        OO     OO     OO   OO  OO  OO OO   OO OO O OO OO
OO   OO OO   OO   OO OO  OO     OO   OO  OO  OO OO   OO OO O OO OO   OO
OO   OO  OOOOO     OOO  OOOO     OOOOO   OO  OO  OOOOO  OO   OO  OOOOO

	(c) 2018 Scott Ferguson
	This code is licensed under MIT license (see LICENSE file for details)
*/

#include "Metronome.h"

/// <summary>Test for expiration of the timer interval.</summary>
/// <returns>True if the timer interval has expired.</returns>
/// <remarks>
/// The timer is reset for the next interval when the test for expiration returns true.
/// The timer is most conveniently referenced by testing the object itself as a boolean expression.
/// </remarks>
bool Metronome::Test()
{
	uint32_t t = millis();
	if (t - LastTime <= PeriodMS)
		return false;
	LastTime = t;
	return true;
}

/// <summary>Test for expiration of the timer interval.</summary>
/// <returns>True if the timer interval has expired.</returns>
/// <remarks>
/// The timer is reset for the next interval when the test for expiration returns true.
/// The timer is most conveniently referenced by testing the object itself as a boolean expression.
/// </remarks>
bool Micronome::Test()
{
	uint32_t t = micros();
	if (t - LastTime <= PeriodMicroS)
		return false;
	LastTime = t;
	return true;
}
