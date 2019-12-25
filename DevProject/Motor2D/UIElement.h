#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__

// TODO 1: Create your structure of classes

#include "p2Point.h"

struct SDL_Texture;
struct SDL_Rect;
class j1Module;

enum class UI_type
{
	FAIL = -1,
	BUTTON,
	TEXT,
	STATIC_IMAGE,
	WINDOW,
	SCROLL_BAR,
	MAX
};


class UIElement
{
public:
	UIElement();
	UIElement(int x, int y, UIElement* father, bool dragable, UI_type type);
	virtual ~UIElement();

	virtual bool Start();
	bool Update();
	virtual bool PostUpdate();
	virtual bool Draw();

	UIElement* GetFather();
	bool MouseUnderElement(int x, int y);

	void Drag(int x, int y);

protected:
	virtual void Move();
	virtual void HandleInput();


public:
	SDL_Rect* my_box;
	//F8 to display UI "colliders"
	bool show_my_box;


	iPoint position;
	iPoint local_position;
	UI_type type;

	bool to_delete;
	bool started;

	bool dragable;
	bool focused;

	SDL_Texture* texture;
	UIElement* father;

};

#endif  //__ELEMENT_H__

