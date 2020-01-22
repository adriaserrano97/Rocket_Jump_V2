#ifndef __INPUT_TEXT_H__
#define __INPUT_TEXT_H__

#include "Text.h"

struct _TTF_Font;

class InputText : public Text
{
public:
	InputText(int x, int y, UIElement* father, _TTF_Font* font, p2SString& text, bool dragable, p2SString& name);
	~InputText();

	void HandleInput();
	
	bool Draw();

private:
	void Speak();

private:
	int cursorPos;
	p2SString previousText;

};

#endif// __INPUT_TEXT_H__
