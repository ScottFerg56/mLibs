/*
@@@@@@@ @@   @@ @@@@@@    @@@
 @@  @@ @@@ @@@  @@  @@    @@
 @@   @ @@@@@@@  @@  @@    @@
 @@ @   @@@@@@@  @@  @@    @@   @@  @@   @@@@@
 @@@@   @@ @ @@  @@@@@     @@   @@  @@  @@   @@
 @@ @   @@   @@  @@  @@    @@   @@  @@  @@@@@@@
 @@     @@   @@  @@  @@    @@   @@  @@  @@
 @@     @@   @@  @@  @@    @@   @@  @@  @@   @@
@@@@    @@   @@ @@@@@@    @@@@   @@@ @@  @@@@@

 Name:		FMBlue.h
 Created:	9/19/2018 11:37:16 AM
 Author:	Scott Ferguson
*/

#ifndef _FMBlue_h
#define _FMBlue_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "FMDebug.h"
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include <Metronome.h>
#include <Applet.h>

class BlueCtrl : public Applet
{
public:
	BlueCtrl(char * name, int8_t cs = 8, int8_t irq = 7, int8_t rst = 4) :
		ble(cs, irq, rst),
		Metro(100), Buffer(""), BufferIndex(0), ServerName(name) {}
	void		Setup();
	void		Run();
	bool		Command(String s);
	bool		Write(String s);

private:
	void		error(char* err);

	String		ServerName;
	Metronome	Metro;
	Adafruit_BluefruitLE_SPI ble;
	bool		Connected;
	String		Buffer;
	uint8_t		BufferIndex;	// current buffer index
};

#endif
