#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "UIElement.h"

struct SDL_Rect;

class Window : public UIElement
{
public:

	Window(int x, int y, UIElement* father, SDL_Rect* rect, bool dragable);
	~Window();

	bool Start();
	bool Update();
	bool Draw();

private:
	SDL_Rect* rect;

};


#endif//__WINDOW_H__ 

