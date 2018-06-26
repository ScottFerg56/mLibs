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

class Debug : public Applet
{
public:
	Debug() : Metro(1000), MetroMetrics(1000) { buffer = ""; }
	void Init(const char* banner, bool wait = false, int debugLED = -1) { Banner = banner; Wait = wait; DebugLED = debugLED; }
	void Setup();
	void Run();
	bool Command(String s);

	String TraceString(int inx);
	void Trace(const char* msg, const char* more = NULL);
	void Trace(const char* msg, String more) { Trace(msg, more.c_str()); }
	String PullTrace(int i);

	void print(String v);
	void print(char v);
	void print(const char* v);

	void print(const char* s, const String &);
	void print(const char* s, const char[]);
	void print(const char* s, char);
	void print(const char* s, unsigned char, int = DEC);
	void print(const char* s, int, int = DEC);
	void print(const char* s, unsigned int, int = DEC);
	void print(const char* s, long, int = DEC);
	void print(const char* s, unsigned long, int = DEC);
	void print(const char* s, double, int = 2);
	void print(const char* s, const Printable&);

	void println(String v);
	void println(char v);
	void println(const char* v);

	void println(const char* s, const String &v);
	void println(const char* s, const char[]);
	void println(const char* s, char);
	void println(const char* s, unsigned char, int = DEC);
	void println(const char* s, int, int = DEC);
	void println(const char* s, unsigned int, int = DEC);
	void println(const char* s, long, int = DEC);
	void println(const char* s, unsigned long, int = DEC);
	void println(const char* s, double, int = 2);
	void println(const char* s, const Printable&);

	bool		Enabled = true;
	bool		Metrics = false;

private:
	bool		Wait;
	const char* Banner;
	Metronome	Metro;
	bool		Connected = false;
	String		buffer;

	Metronome	MetroMetrics;		// metrics loop every 1000 ms
	uint32_t	loopCalls = 0;
	int			DebugLED;

	void CheckConnection();
	bool Ready();
};

extern Debug	debug;

#endif

