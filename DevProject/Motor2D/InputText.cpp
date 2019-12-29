#include "j1App.h"
#include "InputText.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1Input.h"


InputText::InputText(int x, int y, UIElement* father, _TTF_Font* font, p2SString& text, bool dragable, p2SString& name) :
	Text(x, y, father, font, text, dragable, name)
{

	cursorPos = text.Length();
}



void InputText::HandleInput() {

	if (focused == true && App->input->inputTexActivated == false)
	{
		SDL_Rect rect{ position.x, position.y, 0, 0 };
		App->font->CalcSize(string.GetString(), rect.x, rect.y, font_Tex);
		App->input->ActivateTextInput(rect);
	}

	else if (focused == true)
	{
		if (previousText != App->input->getInputText())
		{
			previousText = string = App->input->getInputText();
			texture = App->font->Print(string.GetString(), { 200 }, font_Tex);

			int w, h;
			App->font->CalcSize(string.GetString(), w, h, font_Tex);
		}
		
	}

	else
	{
		App->input->DesactivateTextInput();
	}

}


bool InputText::Draw() {

	App->render->Blit(texture, position.x, position.y);

	p2SString aux = string;
	
	aux.Cut(cursorPos, string.Length());

	SDL_Rect rect{0, 0, 5, 10};

	App->font->CalcSize(aux.GetString(), rect.x, rect.y, font_Tex);

	rect.x += position.x;
	rect.y += position.y;

	App->render->DrawQuad(rect, 0, 0, 0);

	return true;
}