#ifndef __SCROLL_BAR_H__
#define __SCROLL_BAR_H__

#include "UIElement.h"
#include "p2DynArray.h"

class j1Module;
struct SDL_Rect;

class ScrollBar : public UIElement
{
public:
	ScrollBar(int x, int y, UIElement* father, j1Module* listener, SDL_Rect* thumbsIdle, SDL_Rect* thumbsPressed);
	~ScrollBar();

	bool Start();
	bool Update();
	bool Draw();

	void Speaker(j1Module* listener);

public:
	bool pressed;
	SDL_Rect* thumbIdle;
	SDL_Rect* thumbPressed;


private:
	j1Module* listener;

};

#endif // __SCROLL_BAR_H__
