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

#include "FMDebug.h"
#include <FMTime.h>

// The SINGLE instance of the FMDebug Applet for global use
FMDebug	fmDebug;

// The SINGLE instance of Debug for global use
Debug	debug;

/// <summary>A time-stamped entry for logging event messages.</summary>
struct TRACE
{
	int64_t		ms;		// ms timestamp for trace event
	const char	*msg;	// static main message/label for event
	char		*more;	// additional dynamic (malloc'd) text info for event
};

#define TraceCount 60

uint TraceHead = 0;			// the head of the log entries in the Traces array
uint TraceTail = 0;			// the tail of the log entries in the Traces array
TRACE Traces[TraceCount];	// a circular buffer to hold TRACE log entries

/// <summary>Creates a string, suitable for output, of a TRACE log entry.</summary>
/// <param name="inx">The zero-based index of the desired log entry, oldest first. Must be known as valid.</param>
/// <returns>The log entry string.</returns>
/// <remarks>
/// For internal use only.
/// </remarks>
String FMDebug::TraceString(uint inx)
{
	// format the timestamp and base message
	String s = "[" + FMDateTime(Traces[inx].ms).ToString() + "] " + Traces[inx].msg;
	// add the additional string info, if present
	if (Traces[inx].more)
		s += Traces[inx].more;
	return s;
}

/// <summary>Output a TRACE message and log it.</summary>
/// <param name="msg">A constant string message.</param>
/// <param name="more">An additional optional string parameter which must be copied to dynamically allocated memory.</param>
void FMDebug::Trace(const char* msg, const char* more)
{
	// free up any alloc'd memory from a previous use of this TRACE entry in the circular buffer
	if (Traces[TraceHead].more)
		free(Traces[TraceHead].more);
	// record the timestamp
	Traces[TraceHead].ms = FMDateTime::NowMillis();
	// and the main message
	Traces[TraceHead].msg = msg;
	if (more)
	{
		// allocate, copy and record the optional extended message
		char* m = (char*)malloc(strlen(more) + 1);
		strcpy(m, more);
		Traces[TraceHead].more = m;
	}
	else
	{
		// no extended message, so clear it
		Traces[TraceHead].more = NULL;
	}
	// output the TRACE message
	debug.println(TraceString(TraceHead));
	// advance the head through the circular buffer
	if (++TraceHead >= TraceCount)
		TraceHead = 0;
	if (TraceHead == TraceTail)
	{
		// advance the tail through the circular buffer
		if (++TraceTail >= TraceCount)
			TraceTail = 0;
	}
}

/// <summary>Creates a string, suitable for output, of a TRACE log entry.</summary>
/// <param name="i">The zero-based index of the desired log entry, oldest first.</param>
/// <returns>The log entry string, or an empty string if there is no entry at that index.</returns>
/// <remarks>
/// The log entry index is zero for the oldest entry in the log and limited to TraceCount,
/// the maximum number of entries in the log.
/// </remarks>
String FMDebug::PullTrace(uint i)
{
	i = (TraceTail + i) % TraceCount;
	if (i == TraceHead)
		return "";
	return TraceString(i);
}

/// <summary>Initialize (before adding to App).</summary>
/// <param name="banner">The banner to be displayed on the serial output once connected.</param>
/// <param name="wait">True if Setup should wait for Serial connection before proceeding.</param>
/// <param name="debugLED">The LED pin to be toggled periodically as a heartbeat sign of life. (-1 if none.)</param>
void FMDebug::Init(const char* banner, bool wait, int debugLED)
{
	Banner = banner; Wait = wait; DebugLED = debugLED;
}

/// <summary>One-time Setup initialization for the Applet.</summary>
/// <remarks>
/// If 'wait' was specified as true during Init(), this method will not return until
/// the Serial device makes a connection. This is useful when debugging so that
/// other Applets can output important status info during their Setup.
/// For that reason, the 'debug' Applet should be the first added to the App.
/// </remarks>
void FMDebug::Setup()
{
	if (DebugLED != -1)
	{
		pinMode(DebugLED, OUTPUT);
		digitalWrite(DebugLED, HIGH);
	}
	if (Wait)
	{
		while (!CheckConnection())
			;
	}
}

/// <summary>Periodically poll activities for the Applet.</summary>
void FMDebug::Run()
{
	// polling the Serial device can be quite costly in processor time,
	// so we only check periodically using a Metronome timer
	if (Timer)
	{
		// nothing to do if we're not connected
		if (CheckConnection())
		{
			// check for new Serial data
			while (Serial.available())
			{
				char c = Serial.read();
				if (c == ';' || c == '\n' || c == '\r')
				{
					if (Buffer.length() > 0)
					{
						// hit terminator with non-empty buffer
						// pass it to the Parent App who will vector it to the appropriate Applet
						// (this may eventually come back to us as our own Input)
						Parent->Input(Buffer);
						// clear the buffer
						// Note: this does not reduce the reserve capacity of the String in the current implementation
						Buffer = "";
					}
				}
				else
				{
					// buffer the character and keep looking for terminator
					// Note: the String capacity will only grow as needed above our initial reserve amount (so maybe never)
					Buffer.concat(c);
				}
			}
		}
	}

	if (Metrics & Ready())
	{
		// count number of calls
		++LoopCalls;
	}

	if (MetricsTimer)
	{
		// Metrics and LED feedback only once per second
		if (DebugLED != -1)
		{
			// toggle the debug LED as a heartbeat sign of life
			digitalWrite(DebugLED, !digitalRead(DebugLED));
		}
		if (Metrics & Ready())
		{
			// output the number of calls in the last second and the average loop duration
			debug.print("[", FMDateTime::Now().ToString());
			debug.println("] calls: ", LoopCalls);
			debug.println("..loop dur: ", 1000000L / LoopCalls);
			// clear the count
			LoopCalls = 0;
			return;
		}
	}
}

/// <summary>Process a Command string.</summary>
/// <param name="s">The Command string.</param>
/// <remarks>
/// The first character determines the action:
///		'q' - Toggle the Quiet setting, which suppresses debug print output.
///		'm' - Toggle the Metrics setting, which outputs periodic loop performance metrics.
///		'l' - Dump the Trace log.
/// </remarks>
void FMDebug::Command(const String& s)
{
	switch (s[0])
	{
	case 'q':
		// Toggle the Quiet setting, which suppresses debug print output
		Quiet = !Quiet;
		break;
	case 'm':
		// Toggle the Metrics setting, which outputs periodic loop performance metrics
		Metrics = !Metrics;
		break;
	case 'l':
		{
			// Dump the Trace log
			debug.println("<<<<");
			for (int i = 0; ; ++i)
			{
				String s = PullTrace(i);
				if (s.length() == 0)
					break;
				debug.println(s);
			}
			debug.println(">>>>");
		}
		break;
	default:
		debug.println("invalid debug input: ", s[0]);
		break;
	}
}

/// <summary>Determine if the debug object is Ready for output.</summary>
/// <returns>True if the Serial device is connected and output is not suppressed.</returns>
bool FMDebug::Ready()
{
	return Connected && !Quiet;
}

/// <summary>Check for a Serial connection.</summary>
/// <returns>True if the Serial device is (or has been) connected.</returns>
/// <remarks>
/// There appears to be no reliable way to determine if the Serial connection is ever broken.
/// So this method only tests the underlying connection state if a connection has not yet been made.
/// When called after a connection has been made, this will quickly return true.
/// </remarks>
bool FMDebug::CheckConnection()
{
	if (!Connected && Serial)
	{
		Connected = true;
		if (Connected)
		{
			Serial.begin(9600);			// ignored in USB serial comm??
			debug.println(Banner);		// print our banner!
		}
	}
	return Connected;
}

size_t FMDebug::write(uint8_t c) { if (Ready()) { Serial.write(c); } }
size_t FMDebug::write(const uint8_t *buffer, size_t size) { if (Ready()) { Serial.write(buffer, size); } }

// implementations follow for a variety of print/ln functions for debug output.

void Debug::print(const char s[], const __FlashStringHelper *v) { print(s); print(v); }
void Debug::print(const char s[], const String& v) { print(s); print(v); }
void Debug::print(const char s[], const char v[]) { print(s); print(v); }
void Debug::print(const char s[], char v) { print(s); print(v); }
void Debug::print(const char s[], unsigned char v, int p) { print(s); print(v, p); }
void Debug::print(const char s[], int v, int p) { print(s); print(v, p); }
void Debug::print(const char s[], unsigned int v, int p) { print(s); print(v, p); }
void Debug::print(const char s[], long v, int p) { print(s); print(v, p); }
void Debug::print(const char s[], unsigned long v, int p) { print(s); print(v, p); }
void Debug::print(const char s[], double v, int p) { print(s); print(v, p); }
void Debug::print(const char s[], const Printable& v) { print(s); v.printTo(*this); }

void Debug::println(const char s[], const __FlashStringHelper *v) { print(s); println(v); }
void Debug::println(const char s[], const String& v) { print(s); println(v); }
void Debug::println(const char s[], const char v[]) { print(s); println(v); }
void Debug::println(const char s[], char v) { print(s); println(v); }
void Debug::println(const char s[], unsigned char v, int p) { print(s); println(v, p); }
void Debug::println(const char s[], int v, int p) { print(s); println(v, p); }
void Debug::println(const char s[], unsigned int v, int p) { print(s); println(v, p); }
void Debug::println(const char s[], long v, int p) { print(s); println(v, p); }
void Debug::println(const char s[], unsigned long v, int p) { print(s); println(v, p); }
void Debug::println(const char s[], double v, int p) { print(s); println(v, p); }
void Debug::println(const char s[], const Printable& v) { print(s); v.printTo(*this); println(); }
