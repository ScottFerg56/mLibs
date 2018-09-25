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

 Author:	Scott Ferguson
*/

#include "FMStepper.h"

/// <summary>One-time Setup initialization for the Applet.</summary>
void FMStepper::Setup()
{
	// setup IO pins
	if (LimitPin != -1)
		pinMode(LimitPin, INPUT_PULLUP);

	// start moving toward limit switch to initialize home position
	Calibrate();
}

/// <summary>Periodically poll activities for the Applet.</summary>
/// <remarks>Run frequently to keep the Stepper moving on time.</remarks>
void FMStepper::Run()
{
	// check hitting the limit switch
	if (LimitPin != -1)
	{
		bool lim = digitalRead(LimitPin) == LOW;
		if (AtLimit != lim)
		{
			// there's a change
			AtLimit = lim;
			if (AtLimit && GetDistanceToGo() < 0.0)
			{
				// hit limit while moving toward it
				// NOTE: a mechanical switch may bounce while moving away!
				SetCurrentPosition(0);				// (re)calibrate home position
				SendProp(Prop_Position);			// notify the controller
				SendProp(Prop_TargetPosition);		// side effect!
				if (Calibrating)
				{
					Calibrating = false;
					Calibrated = true;
					SendProp(Prop_Calibrated);		// if we were actively calibrating, notify the controller
				}
				debug.println(Name, " Hit Limit");
			//	debug.println("..secs: ", GetLastMoveTime());
			}
		}
	}

	// nudge the Stepper
	RunStatus status = Step();
	if (status != LastStatus)
	{
		// status change
		LastStatus = status;
		if (status == ReachedGoal)
		{
			debug.println(Name, " Reached Goal");
		//	debug.println("..secs: ", GetLastMoveTime());
			SendProp(Prop_Position);		// notify the controller
		}
	}

	if (Timer)
	{
		// check for interesting changes and notify the controller
		if (GetDistanceToGo() != 0)
		{
			SendProp(Prop_Position);
		}

		float speed = GetSpeed();
		if (speed != LastSpeed)
		{
			LastSpeed = speed;
			SendProp(Prop_Speed);
		}
	}
}

/// <summary>Set a property value.</summary>
/// <param name="prop">The property to set.</param>
/// <param name="v">The value to set.</param>
bool FMStepper::SetProp(char prop, const String& v)
{
	switch ((Properties)prop)
	{
	case Prop_Position:
		SetCurrentPosition(v.toFloat());
		SendProp(Prop_TargetPosition);		// side effect!
		break;
	case Prop_Acceleration:
		SetAcceleration(v.toFloat());
		break;
	case Prop_Speed:
		// no need to ever actually set Speed
		return false;
	case Prop_MaxSpeed:
		SetMaxSpeed(v.toFloat());
		break;
	case Prop_SpeedLimit:
		SetSpeedLimit(v.toFloat());
		break;
	case Prop_Velocity:
		SetVelocity(v.toFloat());
		SendProp(Prop_MaxSpeed);		// notify controller of MaxSpeed change
		break;
	case Prop_Calibrated:
		if (LimitPin != -1 && v[0] != '1' && v[0] != 't')
		{
			Calibrated = false;
			SendProp(Prop_Calibrated);	// notify the controller of change
			Calibrate();
		}
		break;
	case Prop_TargetPosition:
		SetTargetPosition(v.toFloat());
		break;
	case Prop_MicrosPerStep:
		SetMicrosPerStep(v.toInt());
		break;
	default:
		return false;
	}
	return true;
}

/// <summary>Get a property value as a string.</summary>
/// <param name="prop">The property to get.</param>
String FMStepper::GetProp(char prop)
{
	switch (prop)
	{
	case Prop_Position:
		return String(GetCurrentPosition());
	case Prop_Acceleration:
		return String(GetAcceleration());
	case Prop_Speed:
		return String(GetSpeed());
	case Prop_MaxSpeed:
		return String(GetMaxSpeed());
	case Prop_SpeedLimit:
		return String(GetSpeedLimit());
	case Prop_Calibrated:
		return String(Calibrated ? 1 : 0);
	case Prop_TargetPosition:
		return String(GetTargetPosition());
	case Prop_MicrosPerStep:
		return String(GetMicrosPerStep());
	case Prop_Velocity:	// write-only
	default:
		return (String)NULL;
	}
}

/// <summary>Moves the stepper as required toward any target position that may be set.</summary>
/// <returns>The updated state of stepper movement.</returns>
/// <remarks>
/// Based on the time interval since the last call, the stepper may be moved at most one step toward any set target position.
/// Run() should be called frequently enough to achieve the desired speed and acceleration.
/// The call to Run() that achieves the target position will return a status of ReachedGoal.
/// Subsequent calls, with no intervening movement directives, will return a status of Stopped.
/// </remarks>
FMStepper::RunStatus FMStepper::Step()
{
	if (!IsMoving)
	{
		return Stopped;
	}

	// don't let the stepper move beyond the set limits
	long dist = Stepper->distanceToGo();
	if (dist != 0 && !Calibrating)
	{
		if (Stepper->currentPosition() >= MaxLimit && dist > 0
			|| Stepper->currentPosition() <= MinLimit && dist < 0)
		{
			return ReachedGoal;
		}
	}

	// move the stepper
	if (!Stepper->run())
	{
		// movement is done
		IsMoving = false;
		// record the stop time
		MoveStopTime = millis();
		// status depends on if we reached the target
		if (Stepper->distanceToGo() == 0)
			return ReachedGoal;
		return Stopped;
	}
	return Moving;
}

/// <summary>Stops the stepper as quickly as possible.</summary>
/// <remarks>
/// Sets a new target position that causes the stepper to stop as quickly as possible, using the current speed and acceleration parameters.
/// </remarks>
void FMStepper::Stop()
{
	Stepper->stop();
}

/// <summary>Get the target position.</summary>
/// <returns>The target position, in logical units.</returns>
float FMStepper::GetTargetPosition()
{
	return Stepper->targetPosition() / StepsPerUnit;
}

/// <summary>Sets the stepper moving toward a new target position.</summary>
/// <param name="position">The new target position, in logical units.</param>
void FMStepper::SetTargetPosition(float position)
{
	// scale and limit the new target
	long goal = (long)(position * StepsPerUnit);
	if (goal > MaxLimit)
		goal = MaxLimit;
	else if (goal < MinLimit)
		goal = MinLimit;
	// set it moving
	Stepper->moveTo(goal);
	IsMoving = true;
	MoveStartTime = millis();
}

/// <summary>Get the current position.</summary>
/// <returns>The current position, in logical units.</returns>
float FMStepper::GetCurrentPosition()
{
	return Stepper->currentPosition() / StepsPerUnit;
}

/// <summary>Set the current position.</summary>
/// <param name="position">The new current position, in logical units.</param>
/// <remarks>
/// Has the AccelStepper side effect of setting both the target position and the current position and stopping movement.
/// Should probably best be used only when stopped.
/// </remarks>
void FMStepper::SetCurrentPosition(float position)
{
	Stepper->setCurrentPosition((long)(position * StepsPerUnit));
	// this will stop a current movement in progress
	IsMoving = false;
	MoveStopTime = millis();
}

/// <summary>Get the acceleration setting to be used by moves.</summary>
/// <returns>The acceleration, in logical units.</returns>
float FMStepper::GetAcceleration()
{
	return Acceleration / StepsPerUnit;
}

/// <summary>Set the acceleration setting to be used by moves.</summary>
/// <param name="accel">The desired acceleration, in logical units.</param>
void FMStepper::SetAcceleration(float accel)
{
	// record the Acceleration value, since we can't recover it from the AccelStepper implementation
	Acceleration = accel * StepsPerUnit;
	Stepper->setAcceleration(Acceleration);
}

/// <summary>Get the speed of current movement.</summary>
/// <returns>The speed of current movement, in logical units.</returns>
float FMStepper::GetSpeed()
{
	return Stepper->speed() / StepsPerUnit;
}

/// <summary>Get the maximum speed used for movement.</summary>
/// <returns>The maximum speed, in logical units.</returns>
float FMStepper::GetMaxSpeed()
{
	return Stepper->maxSpeed() / StepsPerUnit;
}

/// <summary>Set the maximum speed used for movement.</summary>
/// <param name="speed">The desired maximum speed used for movement, in logical units.</param>
/// <remarks>
/// Step movement will accelerate to this speed.
/// </remarks>
void FMStepper::SetMaxSpeed(float speed)
{
	speed = abs(speed);
	if (SpeedLimit != 0 && speed > SpeedLimit)
		speed = SpeedLimit;
	speed *= StepsPerUnit;
	Stepper->setMaxSpeed(speed);
}

/// <summary>Get the upper limit for speeds allowed for movement.</summary>
/// <returns>The maximum value allowed for setting speeds used for movement, in logical units.</returns>
float FMStepper::GetSpeedLimit()
{
	return SpeedLimit;
}

/// <summary>Set the upper limit for speeds allowed for movement.</summary>
/// <param name="speed">The maximum value allowed for setting speeds used for movement, in logical units.</param>
void FMStepper::SetSpeedLimit(float speed)
{
	SpeedLimit = abs(speed);
}

void FMStepper::SetVelocity(float velocity)
{
	// the input value is a desired velocity signed for direction
	if (velocity == 0)
	{
		Stop();	// time to stop
		return;
	}
	// set the non-directional speed
	SetMaxSpeed(abs(velocity));
	// set a target at the appropriate limit to establish direction and get moving
	SetTargetPosition(velocity > 0 ? MaxLimit * StepsPerUnit : -MaxLimit * StepsPerUnit);
}

void FMStepper::Calibrate()
{
	if (LimitPin == -1)
	{
		Calibrated = true;		// no limit switch, just fake it!
		return;
	}
	Calibrating = true;
	SetMaxSpeed(GetSpeedLimit());
	// set it moving toward the limit switch
	Stepper->moveTo(-2000000000L);
	IsMoving = true;
	MoveStartTime = millis();
}

/// <summary>Set the positional limits for movement.</summary>
/// <param name="min">The minimum positional value for movement, in logical units.</param>
/// <param name="max">The maximum positional value for movement, in logical units.</param>
void FMStepper::SetLimits(float min, float max)
{
	MinLimit = min * StepsPerUnit;
	MaxLimit = max * StepsPerUnit;
}

/// <summary>Get the scale factor in steps per unit.</summary>
/// <returns>The scale factor in steps per unit.</returns>
float FMStepper::GetScale()
{
	return StepsPerUnit;
}

/// <summary>Get the scale factor in steps per unit.</summary>
/// <param name="scale">The scale factor in steps per unit.</param>
void FMStepper::SetScale(float scale)
{
	StepsPerUnit = scale;
}

/// <summary>Get the minimum number of microseconds per step.</summary>
/// <returns>The microseconds per step.</returns>
uint32_t FMStepper::GetMicrosPerStep()
{
	return 1000000L / Stepper->maxSpeed();
}

/// <summary>Set the minimum number of microseconds per step.</summary>
/// <param name="usPerStep">The microseconds per step.</param>
/// <remarks>
/// This is an alternative way of controlling MaxSpeed, most useful
/// when monitoring or establishing the performance characteristics of a stepper.
/// </remarks>
void FMStepper::SetMicrosPerStep(uint32_t usPerStep)
{
	Stepper->setMaxSpeed(1000000.0 / usPerStep);
}

/// <summary>Get the duration of the last completed move.</summary>
/// <returns>The duration, in milliseconds, of the last completed move.</returns>
float FMStepper::GetLastMoveTime()
{
	return (MoveStopTime - MoveStartTime) / 1000.0;
}

/// <summary>Get the remaining distance to be traveled for the current move.</summary>
/// <returns>The distance remaining, in logical units.</returns>
float FMStepper::GetDistanceToGo()
{
	return Stepper->distanceToGo() / StepsPerUnit;
}
