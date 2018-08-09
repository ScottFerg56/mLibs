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

void App::Run()
{
	// run all of the Applets
	Applet* a = List;
	while (a != NULL)
	{
		a->Run();
		a = a->Next;
	}
}

bool App::Command(String s)
{
	// process through all of the Applets until one successfully processes it
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
bool App::Command(String s, float v)
{
	return Command(s + String(v));
}

Applet*	App::FindApplet(String name)
{
	Applet* a = List;
	while (a != NULL)
	{
		if (a->Name == name)
			return a;
		a = a->Next;
	}
	debug.println("Applet not found: ", name);
	return NULL;
}
