#ifndef __SCROLL_BAR_H__
#define __SCROLL_BAR_H__

#include "UIElement.h"
#include "p2DynArray.h"

class j1Module;
struct SDL_Rect;

class ScrollBar : public UIElement
{
public:
	ScrollBar(int x, int y, UIElement* father, j1Module* listener, SDL_Rect* thumbsIdle, SDL_Rect* thumbsPressed, bool vertical, p2SString &name);
	~ScrollBar();

	bool Start();
	bool Draw();

	void Speak();

private:
	void Move();
	void HandleInput();

public:
	bool pressed;
	const SDL_Rect* thumbIdle;
	const SDL_Rect* thumbPressed;
	int value;

private:
	j1Module* listener;
	bool vertical;

};

#endif // __SCROLL_BAR_H__
