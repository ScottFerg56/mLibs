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

#include "FMBlue.h"

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

	ble.println(String("AT+GAPDEVNAME=") + ServerName);
	if (!ble.waitForOK())
	{
		error("BLE error setting name");
	}

	ble.verbose(false);  // debug info is a little annoying after this point!

	// Set Bluefruit to DATA mode
//	debug.println("BLE to DATA mode");
	ble.setMode(BLUEFRUIT_MODE_DATA);

//	debug.println("BLE timeout: ", ble.getTimeout());	// 250
	ble.setTimeout(500);
}

void BlueCtrl::Run()
{
	if (Metro)
	{
		// check for connection
		bool newConnected = ble.isConnected();
		if (newConnected != Connected)
		{
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

		if (!Connected)
			return;

		// check for new BLE data
		// packet format: [ '=' | data length byte | section byte | data ]
		// data part may start with a section command character
		while (ble.available())
		{
			char b = ble.read();
			//	debug.println("BLE char: ", (int)b, HEX);
			if (b == ';' || b == '\n' || b == '\r')
			{
				if (Buffer.length() > 0)
				{
					Parent->Command(Buffer);
					Buffer = "";
					BufferIndex = 0;
				}
			}
			else
			{
				Buffer += b;
			}
		}
	}
}

bool BlueCtrl::Write(String s)
{
	if (!Connected)
		return false;
	ble.write(s.c_str());
	return true;
}

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
					if (s.length() > 2)
						Write(s.substring(2) + ";");
					break;
				case 'i':
					// Print BLE information
					debug.println("BLE info:");
					ble.info();
					break;
				case 'd':
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

void BlueCtrl::error(char* err)
{
	debug.println(err);
	while (1);
}
