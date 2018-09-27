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

	(c) 2018 Scott Ferguson
	This code is licensed under MIT license (see LICENSE file for details)
*/

#ifndef _FMBlue_h
#define _FMBlue_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <FMDebug.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <Metronome.h>
#include <Applet.h>

/// <summary>An Adafruit Bluefruit Applet implementation.</summary>
/// <remarks>
/// BlueCtrl implements Bluetooth bidirectional communication via strings of clear text
/// (terminated with ';' or CR or LF) for the Adafruit Bluefruit interfaces.
/// </remarks>
class FMBlue : public Applet
{
public:
	/// <summary>Constructor.</summary>
	/// <param name="prefix">The character code to associate with this Applet.</param>
	/// <param name="name">The name to assign to this Bluetooth device.</param>
	/// <param name="cs">The SPI_CS pin for Bluetooth hardware connection.</param>
	/// <param name="irq">The SPI_IRQ pin for Bluetooth hardware connection.</param>
	/// <param name="rst">The SPI_RST pin for Bluetooth hardware connection. Set to -1 if unused.</param>
	FMBlue(char prefix, char* servername, int8_t cs = 8, int8_t irq = 7, int8_t rst = 4) :
		Applet(prefix), ble(cs, irq, rst), Timer(100), ServerName(servername) { Name = "Bluetooth"; Buffer.reserve(10); }

	void		Setup();
	void		Run();
	void		Command(const String& s);
	bool		Write(const String& s);
	bool		Output(const String& s);

private:
	char*		ServerName;			// The name to be assigned to the Bluetooth server
	Metronome	Timer;				// Timer to be used for polling the Bluetooth interface
	Adafruit_BluefruitLE_SPI ble;	// The Adafruit Bluefruit device
	bool		Connected = false;	// Record of the last known Connected state for the Bluetooth device
	String		Buffer;				// Buffer to receive characters from the Bluetooth device
};

#endif
