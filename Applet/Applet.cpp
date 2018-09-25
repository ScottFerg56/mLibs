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
/// The string is passed to the Input function of the Applet whose Prefix matches the first character of the string.
/// </remarks>
bool App::Input(const String& s)
{
	debug.println("Input: ", s);
	if (s.length() > 1)
	{
		Applet* a = List;
		while (a != NULL)
		{
			if (a->Prefix == s[0])
			{
				a->Input(s.substring(1));
				return true;
			}
			a = a->Next;
		}
	}
	debug.println("Invalid App input: ", s);
	return false;
}

///	<summary>Output string to the outside world using the specified OutputApplet.</summary>
/// <param name="s">The data string to be output.</param>
/// <returns>True if the output succeeded.</returns>
bool App::Output(const String& s)
{
	debug.println("Output: ", s);
	if (OutputApplet != NULL)
	{
		return OutputApplet->Output(s);
	}
	return false;
}

///	<summary>Find the (first) Applet with the specified Name.</summary>
/// <param name="name">The Name to search for.</param>
/// <returns>The Applet found, or NULL if none was found with the specified Name.</returns>
Applet*	App::FindApplet(const char* name)
{
	Applet* a = List;
	while (a != NULL)
	{
		if (strcmp(a->Name, name) == 0)
			return a;
		a = a->Next;
	}
//	debug.println("Applet not found: ", name);
	return NULL;
}

/// <summary>Process an input string.</summary>
/// <param name="s">The input string.</param>
/// <remarks>
/// The first character of the string specifies the action:
///		'?' - remaining characters indicate one or more properties to Output back to the controller
///		'=' - the second character indicates a property and the remainder of tha string a value assign it
///		For any other character, the string is passed on to the Command method
/// </remarks>
void Applet::Input(const String& s)
{

	switch (s[0])
	{
	case '?':
		// send requested value(s) to controller
		if (s.length() > 1)
		{
			for (int i = 1; i < s.length(); i++)	// do all requested properties
			{
				SendProp(s[i]);
			}
			return;
		}
		break;
	case '=':
		// set property value
		if (s.length() > 2)
		{
			String v = s.substring(2);
			if (SetProp(s[1], v))
			{
				debug.print(Name); debug.println(".", String(s[1]) + " <- " + v);
			}
			else
			{
				debug.print(Name); debug.println(": Invalid property: ", s[1]);
			}
			return;
		}
		break;
	default:
		Command(s);
		break;
	}
}

/// <summary>Send a property value to Output.</summary>
/// <param name="prop">The character code for the property to send.</param>
void Applet::SendProp(char prop)
{
	String v = GetProp(prop);			// get the value
	if (v != NULL)
	{
		TrimFloat(v);
		debug.print(Name); debug.println(".", String(prop) + " -> " + v);
		// format and output the value
		Parent->Output(String(Prefix) + "=" + prop + v);
	}
	else
	{
		debug.print(Name); debug.println(": Invalid property: ", prop);
	}
}

void Applet::TrimFloat(String& s)
{
	if (s.indexOf('.') == -1)
		return;
	while (s[s.length() - 1] == '0')
	{
		s.remove(s.length() - 1);
	}
	if (s[s.length() - 1] == '.')
	{
		s.remove(s.length() - 1);
	}
}
