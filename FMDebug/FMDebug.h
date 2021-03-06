/*

OOOOOOO OO   OO OOOOO           OOO
 OO  OO OOO OOO  OO OO           OO
 OO   O OOOOOOO  OO  OO          OO
 OO O   OOOOOOO  OO  OO  OOOOO   OOOO   OO  OO   OOO OO
 OOOO   OO O OO  OO  OO OO   OO  OO OO  OO  OO  OO  OO
 OO O   OO   OO  OO  OO OOOOOOO  OO  OO OO  OO  OO  OO
 OO     OO   OO  OO  OO OO       OO  OO OO  OO  OO  OO
 OO     OO   OO  OO OO  OO   OO  OO  OO OO  OO   OOOOO
OOOO    OO   OO OOOOO    OOOOO   OOOOO   OOO OO     OO
                                                OO  OO
                                                 OOOO

	(c) 2018 Scott Ferguson
	This code is licensed under MIT license (see LICENSE file for details)
*/

/*
An Applet that:
- Encapsulates the Serial print functionality, with labels for easy debug value printing.
- Provides a Trace output mechanism for logging and reviewing events.
- Provides a command routing function for input from the Serial interface.
*/

#ifndef _FMDebug_h
#define _FMDebug_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Metronome.h>
#include <Applet.h>

/// <summary>An Applet implementation of the Serial IO device and other useful testing and debugging functionality.</summary>
/// <remarks>
/// Implements a variety of print/ln functions for debug output.
/// Reads input Serial strings and processes them through the App.Input() method.
/// Provides a Trace mechanism for logging time-stamped events and recalling them at a later time.
/// Provides a mechanism for dumping loop-time metrics for performance evaluation.
/// A SINGLE instance of the Debug class is declared as a global 'debug', to be used throughout the App.
/// The 'debug' Applet should be initialized with a call to Init() before use.
/// The 'debug' Applet should be the first added to the App so that the Serial device is properly Setup
/// for other Applets. 'wait' can be specified during Init() so that Serial output is not missed during debugging.
/// </remarks>
class FMDebug : public Applet
{
public:
	/// <summary>Constructor.</summary>
	FMDebug() : Applet('-'), Timer(1000), MetricsTimer(1000) { Buffer = ""; Name = "Debug"; Buffer.reserve(10); }

	void Init(const char* banner, bool wait = false, int debugLED = -1);

	void Setup();
	void Run();
	void Command(const String& s);

	bool CheckConnection();

	void Trace(const char* msg, const char* more = NULL);
	void Trace(const char* msg, const String& more) { Trace(msg, more.c_str()); }
	String PullTrace(uint i);


	bool		Quiet = false;		// Set to true to suppress debug print output
	bool		Metrics = false;	// Set to true to enable output of loop performance metrics

	size_t		write(uint8_t c);
	size_t		write(const uint8_t *buffer, size_t size);

private:
	String TraceString(uint inx);

	bool		Wait;				// True to wait for Serial connection before leaving Setup
	const char* Banner;				// Banner to output when Serial connection is made
	Metronome	Timer;				// Timer for polling Serial connection and input
	bool		Connected = false;	// Record of the last known Connected state for the Serial device
	String		Buffer;				// Buffer to receive characters from the Serial device

	Metronome	MetricsTimer;		// Timer for the output of loop performance metrics
	uint32_t	LoopCalls = 0;		// Counts number of calls to Run for Metrics
	int			DebugLED;			// The LED pin to be toggled periodically as a sign of life. (-1 if none.)

	bool Ready();
};

// The SINGLE instance of the FMDebug Applet for global use
extern FMDebug	fmDebug;

class Debug : public Print
{
public:
	size_t		write(uint8_t c) { fmDebug.write(c); }
	size_t		write(const uint8_t *buffer, size_t size) { fmDebug.write(buffer, size); }

	// pull in write(str) and write(buf, size) from Print
	using Print::write;
	using Print::print;
	using Print::println;

	void print(const char s[], const __FlashStringHelper *);
	void print(const char s[], const String&);
	void print(const char s[], const char[]);
	void print(const char s[], char);
	void print(const char s[], unsigned char, int = DEC);
	void print(const char s[], int, int = DEC);
	void print(const char s[], unsigned int, int = DEC);
	void print(const char s[], long, int = DEC);
	void print(const char s[], unsigned long, int = DEC);
	void print(const char s[], double, int = 2);
	void print(const char s[], const Printable&);

	void println(const char s[], const __FlashStringHelper *);
	void println(const char s[], const String&);
	void println(const char s[], const char[]);
	void println(const char s[], char);
	void println(const char s[], unsigned char, int = DEC);
	void println(const char s[], int, int = DEC);
	void println(const char s[], unsigned int, int = DEC);
	void println(const char s[], long, int = DEC);
	void println(const char s[], unsigned long, int = DEC);
	void println(const char s[], double, int = 2);
	void println(const char s[], const Printable&);
};

// The SINGLE instance of Debug for global use
extern Debug	debug;

#endif

