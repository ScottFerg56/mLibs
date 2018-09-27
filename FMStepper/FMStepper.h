/*

@@@@@@@ @@   @@  @@@@@     @
 @@  @@ @@@ @@@ @@   @@   @@
 @@   @ @@@@@@@ @@   @@   @@
 @@ @   @@@@@@@  @@     @@@@@@   @@@@@  @@ @@@  @@ @@@   @@@@@  @@ @@@
 @@@@   @@ @ @@   @@@     @@    @@   @@  @@  @@  @@  @@ @@   @@  @@  @@
 @@ @   @@   @@     @@    @@    @@@@@@@  @@  @@  @@  @@ @@@@@@@  @@  @@
 @@     @@   @@ @@   @@   @@    @@       @@  @@  @@  @@ @@       @@
 @@     @@   @@ @@   @@   @@ @@ @@   @@  @@@@@   @@@@@  @@   @@  @@
@@@@    @@   @@  @@@@@     @@@   @@@@@   @@      @@      @@@@@  @@@@
										 @@      @@
										@@@@    @@@@

	(c) 2018 Scott Ferguson
	This code is licensed under MIT license (see LICENSE file for details)
*/

#ifndef _FMStepper_h
#define _FMStepper_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <FMDebug.h>
#include <Metronome.h>
#include <Applet.h>
#include <AccelStepper.h>

/// <summary>An Applet for stepper motor control.</summary>
/// <remarks>
/// FMStepper allows user-friendly logical units to be used with an AccelStepper interface.
/// See the AccelStepper documentation for additional usage information.
/// </remarks>
class FMStepper : public Applet
{
public:
	/// <summary>Constructor.</summary>
	/// <param name="prefix">The character code to associate with this Applet.</param>
	/// <param name="stepsPerUnit">A scaling factor specifying the number of stepper steps per logical unit.</param>
	/// <param name="stepper">A pointer to an AccelStepper object controlling the stepper motor.</param>
	/// <param name="limitPin">Optional. The pin for monitoring a limit switch.</param>
	FMStepper(char prefix, float stepsPerUnit, AccelStepper* stepper, int8_t limitPin = -1) : Applet(prefix), Timer(500)
	{
		Stepper = stepper;
		StepsPerUnit = stepsPerUnit;
		LimitPin = limitPin;
		SpeedLimit = 0;
		MaxLimit = MAXFLOAT;		// initialize to no limits
		MinLimit = -MAXFLOAT;
	}

	void		Setup();
	void		Run();
	String		GetProp(char prop);
	bool		SetProp(char prop, const String& v);

	/// <summary>Status of stepper movement.</summary>
	enum RunStatus
	{
		Stopped,		// Stepper has stopped
		ReachedGoal,	// Stepper just reached goal (next status will be Stopped)
		Moving			// Stepper is still moving
	};

	/// <summary>Properties exposed to the communications interface.</summary>
	/// <remarks>The enum values represent the character codes used in the Input/Output strings.</remarks>
	enum Properties
	{
		Prop_Position = 'p',
		Prop_Acceleration = 'a',
		Prop_Speed = 's',
		Prop_MaxSpeed = 'm',
		Prop_SpeedLimit = 'l',
		Prop_Velocity = 'v',
		Prop_Calibrated = 'c',
		Prop_TargetPosition = 't',
		Prop_MaxLimit = 'x',
		Prop_MinLimit = 'n',
		Prop_MicrosPerStep = 'u',
	};

	RunStatus	Step();
	void		Calibrate();
	void		Stop();
	float		GetTargetPosition();
	void		SetTargetPosition(float position);
	float		GetCurrentPosition();
	void		SetCurrentPosition(float position);
	float		GetAcceleration();
	void		SetAcceleration(float accel);
	float		GetSpeed();
	float		GetMaxSpeed();
	void		SetMaxSpeed(float speed);
	void		SetSpeedLimit(float speed);
	float		GetSpeedLimit();
	void		SetVelocity(float velocity);
	void		SetLimits(float min, float max);
	float		GetScale();
	void		SetScale(float scale);
	uint32_t	GetMicrosPerStep();
	void		SetMicrosPerStep(uint32_t);
	float		GetLastMoveTime();
	float		GetDistanceToGo();

	AccelStepper *Stepper;		// the implementation actually performing stepper movement

protected:
	Metronome	Timer;					// interval timer for feedback
	RunStatus	LastStatus = Stopped;	// most recent status
	float		LastSpeed = 0;			// most recent speed
	int8_t		LimitPin;				// The pin for monitoring a limit switch. (-1 if not supported)
	bool		AtLimit = false;		// limit switch is active/closed
	bool		Calibrated = false;		// limit switch has been reached at least once
	bool		Calibrating = false;

	float		StepsPerUnit;	// scale factor in steps per unit
	float		SpeedLimit;		// in units - limit value for MaxSpeed settings
	float		MaxLimit;		// in units - maximum stepper position value
	float		MinLimit;		// in units - minimum stepper position value
	bool		IsMoving = false; // record of whether we're trying to move the stepper or not
	uint32_t	MoveStartTime;	// record of the start time of the last move, in microseconds
	uint32_t	MoveStopTime;	// record of the stop time of the last move, in microseconds
	float		Acceleration;	// steps per second per second (not scaled units)
};

#endif
