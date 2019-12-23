#include "j1App.h"
#include "Text.h"
#include "j1Render.h"
#include "j1Fonts.h"

Text::Text(int x, int y, p2SString &text, UIElement* father) {

	texture = nullptr;

	started = false;
	to_delete = false;
	dragable = true;
	focused = false;

	string = text;

	this->father = father;

	position.x = x;
	position.y = y;



	type = UI_type::TEXT;
}


Text::~Text() {

	font_Tex = nullptr;
	string.Clear();
	texture = nullptr;
}


bool Text::Start() {

	font_Tex = App->font->default;

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