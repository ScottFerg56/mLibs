/*

OOOOOOO OO   OO   OOOO                    OOO                              O
 OO  OO OOO OOO    OO                      OO                             OO
 OO   O OOOOOOO    OO                      OO                             OO
 OO O   OOOOOOO    OO    OO  OO  OOOO      OO    OOOOO  OOO OO   OOOOO  OOOOOO   OOOOO  OO OOO
 OOOO   OO O OO    OO    OO  OO     OO     OO   OO   OO OOOOOOO OO   OO   OO    OO   OO  OO  OO
 OO O   OO   OO    OO    OO  OO  OOOOO     OO   OO   OO OO O OO OOOOOOO   OO    OOOOOOO  OO  OO
 OO     OO   OO    OO    OO  OO OO  OO     OO   OO   OO OO O OO OO        OO    OO       OO
 OO     OO   OO    OO     OOOO  OO  OO     OO   OO   OO OO O OO OO   OO   OO OO OO   OO  OO
OOOO    OO   OO   OOOO     OO    OOO OO   OOOO   OOOOO  OO   OO  OOOOO     OOO   OOOOO  OOOO

	(c) 2018 Scott Ferguson
	This code is licensed under MIT license (see LICENSE file for details)
 */

#include "FMIvalometer.h"

 /// <summary>One-time Setup initialization for the Applet.</summary>
void FMIvalometer::Setup()
{
	pinMode(FocusPin, INPUT);
	pinMode(ShutterPin, INPUT);
}

/// <summary>Periodically poll activities for the Applet.</summary>
void FMIvalometer::Run()
{
	// operate the cammera shutter
	switch (ShutterAction)
	{
	case Idle:		// nothing to do
		break;
	case Init:		// initialize shutter control pins for use
		{
			uint32_t ms = millis();
			// set focus and shutter pins as outputs and delay for them to set up
		//	debug.println("Intervalometer Init: ", ms);
			pinMode(FocusPin, OUTPUT);
			digitalWrite(FocusPin, HIGH);
			pinMode(ShutterPin, OUTPUT);
			digitalWrite(ShutterPin, HIGH);
			ShutterTime = ms + 20;				// let the pins settle
			ShutterAction = Focus;				// next action
		}
		break;
	case Focus:
		{
			uint32_t ms = millis();
			if (ms >= ShutterTime)				// wait
			{
				// the focus must be triggered and held for some duration
				// before the shutter is triggered, and then both are held until finished
			//	debug.println("Intervalometer Focus: ", ms);
				digitalWrite(FocusPin, LOW);	// ground focus pin to activate
				ShutterTime = ms + FocusDelay;	// delay for camera to focus
				ShutterAction = Shutter;		// next action
			}
		}
		break;
	case Shutter:
		{
			uint32_t ms = millis();
			if (ms >= ShutterTime)				// wait
			{
				// the shutter is triggered after the focus is established,
				// and then both are held until finished
			//	debug.println("Intervalometer Shutter: ", ms);
				digitalWrite(ShutterPin, LOW);	// ground shutter pin to activate
				ShutterTime = ms + ShutterHold;	// delay for camera action
				ShutterAction = Done;			// next action
			}
		}
		break;
	case Done:
		{
			uint32_t ms = millis();
			if (ms >= ShutterTime)					// wait
			{
				// this focus/shutter cycle is complete
				if (Frames == 0 || --Frames == 0)
				{
					// number of frames complete - Intervalometer sequence done!
					SendProp(Prop_Frames);			// notify controller
				//	debug.println("Intervalometer Done: ", ms);
					pinMode(FocusPin, INPUT);		// reset controls to inactive state
					pinMode(ShutterPin, INPUT);
					ShutterAction = Idle;			// next action
				}
				else
				{
					// one more frame complete
					SendProp(Prop_Frames);			// notify controller
				//	debug.println("Intervalometer Frames: ", Frames);
					digitalWrite(FocusPin, HIGH);	// set controls to untriggered state
					digitalWrite(ShutterPin, HIGH);
					// delay for specified interval, or at least time for controls to settle
					if (Interval >= FocusDelay + ShutterHold)
						ShutterTime = ms + Interval - (FocusDelay + ShutterHold);
					else
						ShutterTime = ms + 20;
					ShutterAction = Focus;			// next action
				}
			}
		}
		break;
	default:
		break;
	}
}

/// <summary>Set a property value.</summary>
/// <param name="prop">The property to set.</param>
/// <param name="v">The value to set.</param>
bool FMIvalometer::SetProp(char prop, const String& v)
{
	switch (prop)
	{
	case Prop_FocusDelay:
		FocusDelay = v.toInt();
		break;
	case Prop_ShutterHold:
		ShutterHold = v.toInt();
		break;
	case Prop_Interval:
		Interval = v.toInt();
		break;
	case Prop_Frames:
		Frames = v.toInt();
		if (Interval > 0 && Frames > 0 && ShutterAction == Idle)
		{
			// setting the #frames starts intervalometer function
			ShutterAction = Init;
		}
		break;
	default:
		return false;
	}
	return true;
}

/// <summary>Get a property value as a string.</summary>
/// <param name="prop">The property to get.</param>
String FMIvalometer::GetProp(char prop)
{
	switch (prop)
	{
	case Prop_FocusDelay:
		return String(FocusDelay);
	case Prop_ShutterHold:
		return String(ShutterHold);
	case Prop_Interval:
		return String(Interval);
	case Prop_Frames:
		return String(Frames);
	default:
		return (String)NULL;
	}
}
