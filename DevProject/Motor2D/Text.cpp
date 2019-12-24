#include "j1App.h"
#include "Text.h"
#include "j1Render.h"
#include "j1Fonts.h"


Text::Text(int x, int y, UIElement* father, _TTF_Font* font, p2SString &text, bool dragable) :
	UIElement(x, y, father, dragable, UI_type::TEXT),
	string(text),
	font_Tex(font)
{

	started = false;
	to_delete = false;

	if (father == nullptr)
	{
		local_position = { 0, 0 };
	}

	else
	{
		position.x += father->position.x;
		position.y += father->position.y;
		local_position = { x, y };
	}

}


Text::~Text() {

	font_Tex = nullptr;
	string.Clear();
	texture = nullptr;
}


bool Text::Start() {

	texture = App->font->Print(string.GetString(), { 200 }, font_Tex);

	int w, h;
	App->font->CalcSize(string.GetString(), w, h, font_Tex);

	my_box = new SDL_Rect{ position.x, position.y, w, h };


	started = true;

	return true;
}


bool Text::Draw() {

	App->render->Blit(texture, position.x, position.y);

	return true;
}