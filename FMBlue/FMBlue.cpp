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

/// <summary>One-time Setup initialization for the Bluetooth device.</summary>
void BlueCtrl::Setup()
{
	// Init BLE
//	debug.println("BLE Setup");

	if (!ble.begin(false))	// no VERBOSE mode
	{
		error("No BLE");
	}
//	debug.println("OK");

	// Disable command echo from Bluefruit
	ble.echo(false);

	// set the server name
	ble.println(String("AT+GAPDEVNAME=") + ServerName);
	if (!ble.waitForOK())
	{
		error("BLE error setting name");
	}

	ble.verbose(false);  // debug info is a little annoying after this point!

	// Set Bluefruit to DATA mode
//	debug.println("BLE to DATA mode");
	ble.setMode(BLUEFRUIT_MODE_DATA);

	// set the timeout just a bit longer
//	debug.println("BLE timeout: ", ble.getTimeout());	// 250
	ble.setTimeout(500);
}

/// <summary>Run from the Arduino loop() via App.Run() to poll the Bluetooth device.</summary>
/// <remarks>
/// Periodically poll the Bluetooth device for incoming characters terminated by ';' or CR or LF, building a
/// Command string to be passed to the Parent App Command method for processing by registered Applets.
/// </remarks>
void BlueCtrl::Run()
{
	// polling the Bluetooth device can be quite costly in processor time,
	// so we only check periodically using a Metronome timer
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
					// pass it to the Parent App who will process it through all other Applets
					// (this may eventually come back to us as our own Command)
					Parent->Command(Buffer);
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

/// <summary>Process a Command string, if recognized.</summary>
/// <param name="s">The command string to be processed.</param>
/// <returns>True if recognized.</returns>
/// <remarks>
/// The Command string will be recognized if it starts with 'b' (for Bluetooth) regardless of the contents of the remaining string.
/// Second character:
///		's' - Output the remainder of the string, appended with a ';' terminator, through the Bluetooth device.
///		'i' - Print BLE information to the debug output.
///		'd' - Force Bluetooth disconnect (e.g. for testing purposes).
///		'r' - Perform a factory reset of the Bluetooth device. (Will surely require a subsequent reset of the Arduino.)
/// </remarks>
bool BlueCtrl::Command(String s)
{
	switch (s[0])
	{
		case 'b':
		{
			if (s.length() > 1)
			{
				switch (s[1])
				{
				case 's':
					// Output the remainder of the string, appended with a ';' terminator, through the Bluetooth device.
					if (s.length() > 2)
						Write(s.substring(2) + ";");
					break;
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
						error("BLE factory reset error");
					debug.println("BLE reset done");
					break;
				}
			}
			return true;
		}
	}
	return false;
}

bool BlueCtrl::Output(String s)
{
	return Write(s + ";");
}

/// <summary>Output a fatal error string and enter an infinite loop wait, requiring a reset of the Arduino.</summary>
void BlueCtrl::error(char* err)
{
	debug.println(err);
	while (1);
}
