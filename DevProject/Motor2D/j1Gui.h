#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "UIElement.h"

#define MAX_ELEMENTS 50

#define CURSOR_WIDTH 2

struct _TTF_Font;

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

	// Gui creation functions

	UIElement* CreateButton(int x, int y, UIElement* father, j1Module* listeners[10], SDL_Rect* buttonIdle, SDL_Rect* buttonSelected, SDL_Rect* buttonPressed, bool dragable);
	UIElement* CreateUIWindow(int x, int y, UIElement* father, SDL_Rect* rect, bool dragable);
	UIElement* CreateText(int x, int y, UIElement* father, _TTF_Font* font, p2SString &text, bool dragable);
	UIElement* CreateScrollBar(int x, int y, UIElement* father, j1Module* listener, SDL_Rect* bar, SDL_Rect* thumbsIdle, SDL_Rect* thumbsPressed);

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
