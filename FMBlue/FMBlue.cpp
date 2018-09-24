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

#include "FMBlue.h"

/// <summary>One-time Setup initialization for the Applet.</summary>
void BlueCtrl::Setup()
{
	// Init BLE
//	debug.println("BLE Setup");

	if (!ble.begin(false))	// no VERBOSE mode
	{
		debug.println("No BLE");
		return;
	}
//	debug.println("OK");

	// Disable command echo from Bluefruit
	ble.echo(false);

	// set the server name
	ble.println(String("AT+GAPDEVNAME=") + ServerName);
	if (!ble.waitForOK())
	{
		debug.println("BLE error setting name");
		return;
	}

	ble.verbose(false);  // debug info is a little annoying after this point!

	// Set Bluefruit to DATA mode
	ble.setMode(BLUEFRUIT_MODE_DATA);

	// set the timeout just a bit longer
//	debug.println("BLE timeout: ", ble.getTimeout());	// 250
	ble.setTimeout(500);
}

/// <summary>Periodically poll activities for the Applet.</summary>
/// <remarks>
/// Periodically poll the Bluetooth device for incoming characters terminated by ';' or CR or LF, building an
/// Input string to be passed to the Parent App Input method for processing by registered Applets.
/// </remarks>
void BlueCtrl::Run()
{
	// polling the Bluetooth device can be quite costly in processor time,
	// so we only check periodically using a Metronome Timer
	if (Timer)
	{
		// check for connection
		bool newConnected = ble.isConnected();
		if (newConnected != Connected)
		{
			// Connected status has changed!
			Connected = newConnected;
			if (Connected)
			{
				debug.println("BLE connected");
			}
			else
			{
				debug.println("BLE disconnected");
			}
		}

		// nothing to do if we're not connected
		if (!Connected)
			return;

		// check for new BLE data
		while (ble.available())
		{
			char b = ble.read();
		//	debug.println("BLE char: ", (int)b, HEX);
			if (b == ';' || b == '\n' || b == '\r')
			{
				if (Buffer.length() > 0)
				{
					// hit terminator with non-empty buffer
					// pass it to the Parent App who will vector it to the appropriate Applet
					// (this may eventually come back to us as our own Input)
					Parent->Input(Buffer);
					// clear the buffer
					Buffer = "";
				}
			}
			else
			{
				// buffer the character and keep looking for terminator
				Buffer += b;
			}
		}
	}
}

/// <summary>Output the string through the Bluetooth device.</summary>
/// <param name="s">The string to be output.</param>
/// <returns>True if connected (and the string write was attempted).</returns>
bool BlueCtrl::Write(String s)
{
	// avoid if not Connected
	if (!Connected)
		return false;
	// output the string
	ble.write(s.c_str());
	return true;
}

/// <summary>Process a Command string.</summary>
/// <param name="s">The Command string.</param>
/// <remarks>
/// The first character determines the action:
///		'i' - Print BLE information to the debug output.
///		'd' - Force Bluetooth disconnect (e.g. for testing purposes).
///		'r' - Perform a factory reset of the Bluetooth device. (Will surely require a subsequent reset of the Arduino.)
/// </remarks>
void BlueCtrl::Command(String s)
{
	switch (s[0])
	{
	case 'i':
		// Print BLE information
		debug.println("BLE info:");
		ble.info();
		break;
	case 'd':
		// Force Bluetooth disconnect
		ble.disconnect();
		break;
	case 'r':
		// Perform a factory reset to make sure everything is in a known state
		debug.println("BLE factory reset");
		if (!ble.factoryReset())
			debug.println("BLE factory reset error");
		else
			debug.println("BLE reset done");
		break;
	default:
		debug.println("invalid FMBlue input: ", s[0]);
		break;
	}
}

/// <summary>Output a terminated packet string through the Bluetooth device.</summary>
/// <param name="s">The string to be output.</param>
/// <returns>True if connected (and the string write was attempted).</returns>
bool BlueCtrl::Output(String s)
{
	// add a packet-terminating ';'
	return Write(s + ";");
}
