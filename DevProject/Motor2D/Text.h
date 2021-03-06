#ifndef __TEXT_H__
#define __TEXT_H__

#include "UIElement.h"
#include "p2SString.h"

struct _TTF_Font;

class Text : public UIElement
{
public:
	Text(int x, int y, UIElement* father, _TTF_Font* font, p2SString &text, bool dragable, p2SString &name);
	virtual ~Text();

	bool Start();
	bool Draw();


protected:
	_TTF_Font* font_Tex;

protected:
	p2SString string;
};

#endif// __TEXT_H__