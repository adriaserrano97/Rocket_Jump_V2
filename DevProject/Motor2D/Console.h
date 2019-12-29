#ifndef __j1CONSOLE_H__
#define __j1CONSOLE_H__

#define CONSOLE_ELEMENTS 20
#define MAX_CONSOLE_TEXTS 10

#include "j1Module.h"

// ---------------------------------------------------
class j1Console : public j1Module
{
public:

	j1Console();

	// Destructor
	virtual ~j1Console();

	// Call before first frame
	//bool Start();

	// Called before all Updates
	bool PreUpdate();

	bool Update();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	//bool CleanUp();

	void ListenerUI(UIElement* UI_element);

	void AddTextToConsole(p2SString& string);

private:
	void CreateConsole();
	void DestroyConsole();

	

public:
	bool consoleOpen;


private:
	UIElement* consoleElements[CONSOLE_ELEMENTS];
	UIElement* consoleText[MAX_CONSOLE_TEXTS];
};

#endif // __j1CONSOLE_H__

