#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "UIElement.h"

#define MAX_ELEMENTS 50

#define CURSOR_WIDTH 2

// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods
	// Gui creation functions
	void CreateGUIElement(UI_type type, int x, int y, UIElement* father, p2SString text = "");

	SDL_Texture* GetAtlas() const;

	//UI_Listener
	void ListenerUI(UIElement* UI_element);

private:

	void DragElement();
	void CheckFocusedElements();

private:

	UIElement* elementArray[MAX_ELEMENTS];

	UIElement* elementDragged;

	SDL_Texture* atlas;
	p2SString atlasFileName;

	int mouseDragX;
	int mouseDragY;
};

#endif // __j1GUI_H__
