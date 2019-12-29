#include "j1App.h"
#include "Text.h"
#include "j1Render.h"
#include "j1Fonts.h"


Text::Text(int x, int y, UIElement* father, _TTF_Font* font, p2SString &text, bool dragable, p2SString &name) :
	UIElement(x, y, father, dragable, UI_type::TEXT, name),
	string(text),
	font_Tex(font)
{

	if (father == nullptr)
	{
		local_position = { x, y };
	}

	else
	{
		position.x += father->position.x;
		position.y += father->position.y;
		local_position = { x, y };
	}

}


Text::~Text() 
{
	font_Tex = nullptr;
	string.Clear();
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