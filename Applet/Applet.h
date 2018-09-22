/*
                                                
   @                      @@@              @    
  @@@                      @@             @@    
 @@ @@                     @@             @@    
@@   @@ @@ @@@  @@ @@@     @@    @@@@@  @@@@@@  
@@   @@  @@  @@  @@  @@    @@   @@   @@   @@    
@@@@@@@  @@  @@  @@  @@    @@   @@@@@@@   @@    
@@   @@  @@  @@  @@  @@    @@   @@        @@    
@@   @@  @@@@@   @@@@@     @@   @@   @@   @@ @@ 
@@   @@  @@      @@       @@@@   @@@@@     @@@  
         @@      @@                             
        @@@@    @@@@                            
                                                
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
/// Applets can communicate with each other by sending and processing Commands via their App Parent.
/// Communications Applets can use the App Command mechanism to route incoming commands to the proper destination
/// or process Commands from other Applets to send outgoing data.
/// </remarks>
class App
{
public:
	App() : List(NULL) { }
	void	AddApplet(Applet* applet);
	void	Run();
	bool	Command(String s);
	bool	Output(String s);
	Applet*	FindApplet(String name);
	// An applet used to send data to the outside world
	Applet*	OutputApplet;

protected:
	// The list of Applets added
	Applet*	List;
};

///	<summary>An abstraction to encapsulate behavior for device functionality.</summary>
/// <remarks>
/// The Applet encapsulates and isolates the behavior of a device or logical function.
/// It is Setup once from the main Arduino setup() function.
/// It is Run from the main Arduino loop() to receive slices of time for regular processing.
/// It can receive Commands, from cummunications or other Applets, to implement it's functionality.
/// It can also use the Command method of its Parent App to send commands to other Applets
/// or to communications Applets to send outgoing data.
/// </remarks>
class Applet
{
	friend class App;
public:
	Applet(char prefix) : Prefix(prefix)
	{
	}

	// Invoked when the Applet is added
	virtual void	Setup() {}
	// Invoked from the App Run method
	virtual void	Run() {}
	// Process a command string. Return true if recognized.
	virtual bool	Command(String s) {}
	// Output a string, if applicable
	virtual bool	Output(String s) {}
	// The prefix character for commands
	char			Prefix;
	// An arbitrary Name for the Applet
	String			Name;

protected:
	// The parent App
	App*			Parent;
	// The next Applet in the parent App's list
	Applet*			Next;
};

#endif
