/*

@@@@@@@ @@   @@   @@@@                    @@@                              @
 @@  @@ @@@ @@@    @@                      @@                             @@
 @@   @ @@@@@@@    @@                      @@                             @@
 @@ @   @@@@@@@    @@    @@  @@  @@@@      @@    @@@@@  @@@ @@   @@@@@  @@@@@@   @@@@@  @@ @@@
 @@@@   @@ @ @@    @@    @@  @@     @@     @@   @@   @@ @@@@@@@ @@   @@   @@    @@   @@  @@  @@
 @@ @   @@   @@    @@    @@  @@  @@@@@     @@   @@   @@ @@ @ @@ @@@@@@@   @@    @@@@@@@  @@  @@
 @@     @@   @@    @@    @@  @@ @@  @@     @@   @@   @@ @@ @ @@ @@        @@    @@       @@
 @@     @@   @@    @@     @@@@  @@  @@     @@   @@   @@ @@ @ @@ @@   @@   @@ @@ @@   @@  @@
@@@@    @@   @@   @@@@     @@    @@@ @@   @@@@   @@@@@  @@   @@  @@@@@     @@@   @@@@@  @@@@

	(c) 2018 Scott Ferguson
	This code is licensed under MIT license (see LICENSE file for details)
 */

#ifndef _FMIvalometer_h
#define _FMIvalometer_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <FMDebug.h>
#include <Applet.h>

class FMIvalometer : public Applet
{
public:
	/// <summary>Constructor.</summary>
	/// <param name="prefix">The character code to associate with this Applet.</param>
	/// <param name="focusPin">The output pin used to trigger a focus operation.</param>
	/// <param name="shutterPin">The output pin used to trigger a shutter operation.</param>
	FMIvalometer(char prefix, uint8_t focusPin, uint8_t shutterPin) : Applet(prefix)
	{
		FocusPin = focusPin;
		ShutterPin = shutterPin;
	}

	void		Setup();
	void		Run();
	bool		SetProp(char prop, const String& v);
	String		GetProp(char prop);

	/// <summary>Properties exposed to the communications interface.</summary>
	/// <remarks>The enum values represent the character codes used in the Input/Output strings.</remarks>
	enum Properties
	{
		Prop_FocusDelay = 'd',
		Prop_ShutterHold = 's',
		Prop_Interval = 'i',
		Prop_Frames = 'f',
	};

private:
	/// <summary>The state of a shutter trigger sequence.</summary>
	enum ShutterStatus
	{
		Idle,		// no trigger sequence active
		Init,		// shutter and focus controls initialized
		Focus,		// focus control triggered
		Shutter,	// shutter control triggered
		Done		// controls released
	};

	uint8_t		FocusPin;				// the output pin used to trigger a focus operation
	uint8_t		ShutterPin;				// the output pin used to trigger a shutter operation
	ShutterStatus ShutterAction = Idle;	// next shutter action to take
	uint32_t	ShutterTime;			// in ms - time for next shutter/focus action
	uint		FocusDelay = 150;		// in ms - delay after focus before tripping shutter
	uint		ShutterHold = 50;		// in ms - time to hold shutter signal
	uint		Interval = 0;			// in ms - time between camera frames
	uint		Frames = 0;				// # frames remaining to shoot
};

#endif
