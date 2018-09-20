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

/// <summary>An Adafruit Bluefruit Applet implementation.</summary>
/// <remarks>
/// BlueCtrl implements Bluetooth bidirectional communication via strings of clear text
/// (terminated with ';' or CR or LF) for the Adafruit Bluefruit interfaces.
/// </remarks>
class BlueCtrl : public Applet
{
public:
	/// <summary>Construct with specified server name and hardware control pins.</summary>
	BlueCtrl(char * name, int8_t cs = 8, int8_t irq = 7, int8_t rst = 4) :
		ble(cs, irq, rst), Timer(100), ServerName(name) {}
	void		Setup();
	void		Run();
	bool		Command(String s);
	bool		Write(String s);

private:
	void		error(char* err);

	// The name to be assigned to the Bluetooth server
	String		ServerName;
	// Timer to be used for polling the Bluetooth interface
	Metronome	Timer;
	// The Adafruit Bluefruit device
	Adafruit_BluefruitLE_SPI ble;
	// Record of the last known Connected state for the Bluetooth device
	bool		Connected = false;
	// Buffer to receive characters from the Bluetooth device
	String		Buffer = "";
};

#endif
