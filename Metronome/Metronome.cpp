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

 Author:	Scott Ferguson
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
