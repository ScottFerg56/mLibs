/*
                                                        
@@@@@@@ @@   @@ @@@@@           @@@                     
 @@  @@ @@@ @@@  @@ @@           @@                     
 @@   @ @@@@@@@  @@  @@          @@                     
 @@ @   @@@@@@@  @@  @@  @@@@@   @@@@   @@  @@   @@@ @@ 
 @@@@   @@ @ @@  @@  @@ @@   @@  @@ @@  @@  @@  @@  @@  
 @@ @   @@   @@  @@  @@ @@@@@@@  @@  @@ @@  @@  @@  @@  
 @@     @@   @@  @@  @@ @@       @@  @@ @@  @@  @@  @@  
 @@     @@   @@  @@ @@  @@   @@  @@  @@ @@  @@   @@@@@  
@@@@    @@   @@ @@@@@    @@@@@   @@@@@   @@@ @@     @@  
                                                @@  @@  
                                                 @@@@   
                                                        
 Author:	Scott Ferguson
*/

#include "FMDebug.h"
#include <FMTime.h>

// The SINGLE instance of the Debug Applet to be used throughout the App!
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
String Debug::TraceString(uint inx)
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
void Debug::Trace(const char* msg, const char* more)
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
String Debug::PullTrace(uint i)
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
void Debug::Init(const char* banner, bool wait, int debugLED)
{
	Banner = banner; Wait = wait; DebugLED = debugLED;
}

/// <summary>One-time Setup initialization for the Serial device.</summary>
/// <remarks>
/// If 'wait' was specified as true during Init(), this method will not return until
/// the Serial device makes a connection. This is useful when debugging so that
/// other Applets can output important status info during their Setup.
/// For that reason, the 'debug' Applet should be the first added to the App.
/// </remarks>
void Debug::Setup()
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

///	<summary>Run all of the Applets in the App's list.</summary>
void Debug::Run()
{
	// polling the Serial device can be quite costly in processor time,
	// so we only check periodically using a Metronome timer
	if (Timer)
	{
		// nothing to do if we're not connected
		if (CheckConnection())
		{
			while (Serial.available())
			{
				char c = Serial.read();
				if (c == ';' || c == '\n' || c == '\r')
				{
					// hit terminator with non-empty buffer
					// pass it to the Parent App who will process it through all other Applets
					// (this may eventually come back to us as our own Command)
					Parent->Command(Buffer);
					// clear the buffer
					Buffer = "";
				}
				else
				{
					// buffer the character and keep looking for terminator
					Buffer += c;
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

/// <summary>Process a Command string, if recognized.</summary>
/// <param name="s">The command string to be processed.</param>
/// <returns>True if recognized.</returns>
/// <remarks>
/// The Command string will be recognized if it starts with '-' regardless of the contents of the remaining string.
/// Second character:
///		'q' - Toggle the Quiet setting, which suppresses debug print output.
///		'm' - Toggle the Metrics setting, which outputs periodic loop performance metrics.
///		'l' - Dump the Trace log.
/// </remarks>
bool Debug::Command(String s)
{
	if (s.length() == 0 || s[0] != '-')
		return false;

	if (s.length() > 1)
	{
		switch (s[1])
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
					String s = debug.PullTrace(i);
					if (s.length() == 0)
						break;
					debug.println(s);
				}
				debug.println(">>>>");
			}
			break;
		default:
			return false;
		}
	}
	return true;
}

/// <summary>Determine if the debug object is Ready for output.</summary>
/// <returns>True if the Serial device is connected and output is not suppressed.</returns>
bool Debug::Ready()
{
	return Connected && !Quiet;
}

// implementations follow for a variety of print/ln functions for debug output.

#define PRINTONE { if (!Ready()) return; Serial.print(v); }
#define PRINTTWO { if (!Ready()) return; Serial.print(s); Serial.print(v); }
#define PRINTTHREE { if (!Ready()) return; Serial.print(s); Serial.print(v, p); }

void Debug::print(String v) PRINTONE
void Debug::print(char v) PRINTONE
void Debug::print(const char* v) PRINTONE

void Debug::print(const char* s, const String &v) PRINTTWO
void Debug::print(const char* s, const char v[]) PRINTTWO
void Debug::print(const char* s, char v) PRINTTWO
void Debug::print(const char* s, unsigned char v, int p) PRINTTHREE
void Debug::print(const char* s, int v, int p) PRINTTHREE
void Debug::print(const char* s, unsigned int v, int p) PRINTTHREE
void Debug::print(const char* s, long v, int p) PRINTTHREE
void Debug::print(const char* s, unsigned long v, int p) PRINTTHREE
void Debug::print(const char* s, double v, int p) PRINTTHREE
void Debug::print(const char* s, const Printable& v) { if (!Ready()) return; Serial.print(s); v.printTo(Serial); }

#define PRINTLNONE { if (!Ready()) return; Serial.println(v); }
#define PRINTLNTWO { if (!Ready()) return; Serial.print(s); Serial.println(v); }
#define PRINTLNTHREE { if (!Ready()) return; Serial.print(s); Serial.println(v, p); }

void Debug::println(String v) PRINTLNONE
void Debug::println(char v) PRINTLNONE
void Debug::println(const char* v) PRINTLNONE

void Debug::println(const char* s, const String &v) PRINTLNTWO
void Debug::println(const char* s, const char v[]) PRINTLNTWO
void Debug::println(const char* s, char v) PRINTLNTWO
void Debug::println(const char* s, unsigned char v, int p) PRINTLNTHREE
void Debug::println(const char* s, int v, int p) PRINTLNTHREE
void Debug::println(const char* s, unsigned int v, int p) PRINTLNTHREE
void Debug::println(const char* s, long v, int p) PRINTLNTHREE
void Debug::println(const char* s, unsigned long v, int p) PRINTLNTHREE
void Debug::println(const char* s, double v, int p) PRINTLNTHREE
void Debug::println(const char* s, const Printable& v) { if (!Ready()) return; Serial.print(s); v.printTo(Serial); Serial.println(); }

/// <summary>Check for a Serial connection.</summary>
/// <returns>True if the Serial device is (or has been) connected.</returns>
/// <remarks>
/// There appears to be no reliable way to determine if the Serial connection is ever broken.
/// So this method only tests the underlying connection state if a connection has not yet been made.
/// When called after a connection has been made, this will quickly return true.
/// Otherwise, testing the connection may take considerably more processor time.
/// </remarks>
bool Debug::CheckConnection()
{
	if (!Connected && Serial)
	{
		Connected = true;
		if (Connected)
		{
			Serial.begin(9600);		// ignored in USB serial comm??
			println(Banner);		// print our banner!
		}
	}
	return Connected;
}
