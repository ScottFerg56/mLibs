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

#include "FMDebug.h"
#include <FMTime.h>

Debug	debug;

//extern float BatteryVoltage();
//extern uint8_t USBGetConfiguration(void);

struct TRACE
{
	int64_t		ms;		// ms timestamp for trace event
	const char	*msg;	// static main message/label for event
	char		*more;	// additional dynamic (malloc'd) text info for event
};

#define TraceCount 60

int TraceHead = 0;
int TraceTail = 0;
TRACE Traces[TraceCount];

String Debug::TraceString(int inx)
{
	String s = "[" + FMDateTime(Traces[inx].ms).ToString() + "] " + Traces[inx].msg;
	if (Traces[inx].more)
		s += Traces[inx].more;
	return s;
}

void Debug::Trace(const char* msg, const char* more)
{
	if (Traces[TraceHead].more)
		free(Traces[TraceHead].more);
	Traces[TraceHead].ms = FMDateTime::NowMillis();
	Traces[TraceHead].msg = msg;
	if (more)
	{
		char* m = (char*)malloc(strlen(more) + 1);
		strcpy(m, more);
		Traces[TraceHead].more = m;
	}
	else
	{
		Traces[TraceHead].more = NULL;
	}
	debug.println(TraceString(TraceHead));
	if (++TraceHead >= TraceCount)
		TraceHead = 0;
	if (TraceHead == TraceTail)
	{
		if (++TraceTail >= TraceCount)
			TraceTail = 0;
	}
}

String Debug::PullTrace(int i)
{
	i = (TraceTail + i) % TraceCount;
	if (i == TraceHead)
		return "";
	return TraceString(i);
}

void Debug::Setup()
{
	if (DebugLED != -1)
	{
		pinMode(DebugLED, OUTPUT);
		digitalWrite(DebugLED, HIGH);
	}
	if (Wait)
	{
		//uint8_t usbc1 = USBGetConfiguration();
		while (!Connected)
			CheckConnection();
		//uint8_t usbc2 = USBGetConfiguration();
		//debug.println("usbc1: ", usbc1);
		//debug.println("usbc2: ", usbc2);
	}
}

void Debug::Run()
{
	if (Metro)
	{
		CheckConnection();
		if (Connected)
		{
			while (Serial.available())
			{
				char c = Serial.read();
				if (c == '\n')
				{
					Parent->Command(buffer);
					buffer = "";
				}
				else
				{
					buffer += c;
				}
			}
		}
	}

	if (Metrics & Ready())
	{
		++loopCalls;
	}

	if (MetroMetrics)
	{
		if (DebugLED != -1)
		{
			digitalWrite(DebugLED, !digitalRead(DebugLED));
		}
		if (Metrics & Ready())
		{
			//
			// EVERY SECOND
			//
			debug.print("[", FMDateTime::Now().ToString());
			debug.println("] calls: ", loopCalls);
			debug.println("..loop dur: ", 1000000L / loopCalls);
			loopCalls = 0;
			return;
		}
	}
}

bool Debug::Command(String s)
{
	if (s.length() == 0 || s[0] != '-')
		return false;

	if (s.length() > 1)
	{
		switch (s[1])
		{
		case 'q':
			Enabled = !Enabled;
			break;
		case 'm':
			Metrics = !Metrics;
			break;
		case 'l':
			{
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

bool Debug::Ready()
{
	return Connected && Enabled;
}

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

void Debug::CheckConnection()
{
	if (!Connected && Serial)
	{
		Connected = true;
		if (Connected)
		{
			Serial.begin(9600);		// ignored in USB serial comm??
			println(Banner);
		}
		//if (DebugLED != -1)
		//{
		//	for (int i = 0; i < 8; i++)
		//	{
		//		digitalWrite(DebugLED, HIGH);
		//		delay(50);
		//		digitalWrite(DebugLED, LOW);
		//		delay(50);
		//	}
		//}
	}
}
