/*

   O                      OOO              O
  OOO                      OO             OO
 OO OO                     OO             OO
OO   OO OO OOO  OO OOO     OO    OOOOO  OOOOOO
OO   OO  OO  OO  OO  OO    OO   OO   OO   OO
OOOOOOO  OO  OO  OO  OO    OO   OOOOOOO   OO
OO   OO  OO  OO  OO  OO    OO   OO        OO
OO   OO  OOOOO   OOOOO     OO   OO   OO   OO OO
OO   OO  OO      OO       OOOO   OOOOO     OOO
         OO      OO
        OOOO    OOOO

	(c) 2018 Scott Ferguson
	This code is licensed under MIT license (see LICENSE file for details)
*/

#ifndef _APPLET_h
#define _APPLET_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Applet;

///	<summary>A collection of Applets.</summary>
/// <remarks>
/// The App object holds a list of Applets and provides a mechanism by which they are
/// Setup from the main Arduino setup() and Run from the main Arduino loop().
/// </remarks>
class App
{
public:
	App() : List(NULL) { }
	void	AddApplet(Applet* applet);
	void	Run();
	bool	Input(const String& s);
	bool	Output(const String& s);
	Applet*	FindApplet(const char* name);
	// An applet used to send data to the outside world
	Applet*	OutputApplet = NULL;

protected:
	// The list of Applets added
	Applet*	List;
};

///	<summary>An abstraction to encapsulate behavior for device functionality.</summary>
/// <remarks>
/// The Applet encapsulates and isolates the behavior of a device or logical function.
/// It is Setup once from the main Arduino setup() function.
/// It is Run from the main Arduino loop() to receive slices of time for regular processing.
/// It can receive Input from cummunications Applets to implement it's functionality.
/// It can also use the Output method of its Parent App to send outgoing communications data.
/// </remarks>
class Applet
{
	friend class App;
public:
	Applet(char prefix) : Prefix(prefix)
	{
	}

	/// <summary>One-time Setup initialization for the Applet.</summary>
	virtual void	Setup() = 0;

	/// <summary>Periodically poll activities for the Applet.</summary>
	virtual void	Run() = 0;

	/// <summary>Process an input string.</summary>
	/// <remarks>
	/// Most Applets will support the Command method or the Get/SetProp methods or both.
	/// The Input method can be defined to override that functionality for input processing.
	/// </remarks>
	virtual void	Input(const String& s);

	/// <summary>Output a string, if applicable.</summary>
	/// <remarks>
	/// Applets that provide outgoing communications can implement this Output method
	/// and be set as the OutputApplet on the master App object.
	/// </remarks>
	virtual bool	Output(const String& s) { }

	/// <summary>Process a command string.</summary>
	/// <remarks>
	/// Applets that want to respond to incoming communications, other than property exchange,
	/// can implement this method.
	/// </remarks>
	virtual void	Command(const String& s) { }

	/// <summary>Get a property value as a string.</summary>
	/// <param name="prop">The property to get.</param>
	virtual String	GetProp(char prop) { return (String)NULL; }

	/// <summary>Set a property value.</summary>
	/// <param name="prop">The property to set.</param>
	/// <param name="v">The value to set.</param>
	virtual bool	SetProp(char prop, const String& v) { return false; }

	/// <summary>Send a property value to Output.</summary>
	/// <param name="prop">The character code for the property to send.</param>
	void			SendProp(char prop);

	void			TrimFloat(String& s);

	char			Prefix;		// The prefix character for commands
	char*			Name;		// An arbitrary Name for the Applet

protected:
	App*			Parent;		// The parent App
	Applet*			Next;		// The next Applet in the parent App's list
};

#endif
