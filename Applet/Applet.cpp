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

#include "Applet.h"
#include <FMDebug.h>

///	<summary>Add an Applet to the App's list.</summary>
/// <param name="applet">The Applet to be added.</param>
/// <remarks>The Setup method of the Applet is invoked as it is added.</remarks>
void App::AddApplet(Applet* applet)
{
	// initialize and Setup the Applet
	applet->Parent = this;
	applet->Next = NULL;
	applet->Name = "";
	applet->Setup();
	if (List == NULL)
	{
		// set as the head of the list
		List = applet;
		return;
	}
	// add to the end of the list
	Applet* a = List;
	while (true)
	{
		if (a->Next == NULL)
		{
			a->Next = applet;
			return;
		}
		a = a->Next;
	}
}

///	<summary>Run all of the Applets in the App's list.</summary>
void App::Run()
{
	Applet* a = List;
	while (a != NULL)
	{
		a->Run();
		a = a->Next;
	}
}

///	<summary>Process a command string through all of the Applets until one successfully recognizes it.</summary>
/// <param name="s">The command string to be processed.</param>
/// <returns>True if an Applet recognized the command, otherwise false.</returns>
/// <remarks>
/// The input string is converted to Lower Case!
/// The command is passed to the Command function of each Applet in the order added until one returns true.
/// The mechanism by which Command strings are recognized by a given Applet is arbitrary and should be
/// managed across the application.
/// </remarks>
bool App::Command(String s)
{
	s.toLowerCase();
//	debug.println("Command: ", s);
	Applet* a = List;
	while (a != NULL)
	{
		if (a->Command(s))
			return true;
		a = a->Next;
	}
	debug.println("Invalid command: ", s);
	return false;
}

///	<summary>Process a command string through all of the Applets until one successfully recognizes it.</summary>
/// <param name="s">The command string to be processed.</param>
/// <param name="v">A value to be appended to the command string.</param>
/// <returns>True if an Applet recognized the command, otherwise false.</returns>
/// <remarks>The command is passed to the Command function of each Applet in the order added until one returns true.</remarks>
bool App::Command(String s, float v)
{
	s = s + String(v);
//	debug.println("--> ", s);
	return Command(s);
}

///	<summary>Process a command string through all of the Applets until one successfully recognizes it.</summary>
/// <param name="s">The command string to be processed.</param>
/// <param name="v">A value to be appended to the command string.</param>
/// <returns>True if an Applet recognized the command, otherwise false.</returns>
/// <remarks>The command is passed to the Command function of each Applet in the order added until one returns true.</remarks>
bool App::Command(String s, uint v)
{
	s = s + String(v);
//	debug.println("--> ", s);
	return Command(s);
}

///	<summary>Find the (first) Applet with the specified Name.</summary>
/// <param name="name">The Name to search for.</param>
/// <returns>The Applet found, or NULL if none was found with the specified Name.</returns>
Applet*	App::FindApplet(String name)
{
	Applet* a = List;
	while (a != NULL)
	{
		if (a->Name == name)
			return a;
		a = a->Next;
	}
//	debug.println("Applet not found: ", name);
	return NULL;
}
