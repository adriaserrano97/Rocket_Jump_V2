#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "UIElement.h"
#include "p2DynArray.h"

class j1Module;
struct SDL_Rect;

enum class BUTTON_STATE {

	IDLE,
	SELECTED,
	CLICKED,
	CLICKED_IDLE,
	RELEASED,
	BUTTON_STATE_MAX

};

class Button : public UIElement
{
public:
	Button(int x, int y, UIElement* father, j1Module* listeners[10], SDL_Rect* buttonIdle, SDL_Rect* buttonSelected, SDL_Rect* buttonPressed, bool dragable);
	~Button();

	bool Start();
	bool Draw();

	void Speaker(j1Module* listener);
	void SpeakToAll();

private:
	void HandleInput();

public:
	bool pressed;
	SDL_Rect* button_idle;
	SDL_Rect* button_selected;
	SDL_Rect* button_pressed;

private:

	j1Module* listeners[10];

};

#endif // __BUTTON_H__
