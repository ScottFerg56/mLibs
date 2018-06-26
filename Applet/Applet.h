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

/*
Defines an App as a collection of Applets, each of which is Setup when added
and Run when the App is Run. Applets have access to services, like Command(), offered
by the Parent App.
*/

#ifndef _APPLET_h
#define _APPLET_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Applet;

class App
{
public:
	App() : List(NULL) { }
	void	AddApplet(Applet* applet);
	void	Run();
	bool	Command(String s);
	Applet*	FindApplet(String name);

protected:
	Applet*	List;
};

class Applet
{
	friend class App;
public:
	virtual void	Setup() {}
	virtual void	Run() {}
	virtual bool	Command(String s) {}
	String			Name;

protected:
	App*			Parent;
	Applet*			Next;
};

#endif
