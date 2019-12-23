#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "UIElement.h"

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
	Button(int x, int y, UIElement* father);
	~Button();

	bool Start();
	bool Update();
	bool Draw();

public:
	bool pressed;
	SDL_Rect* button_idle;
	SDL_Rect* button_selected;
	SDL_Rect* button_pressed;

};

#endif // __BUTTON_H__
