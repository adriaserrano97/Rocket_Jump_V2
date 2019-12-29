#include "j1App.h"
#include "InputText.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1Input.h"


InputText::InputText(int x, int y, UIElement* father, _TTF_Font* font, p2SString& text, bool dragable, p2SString& name) :
	Text(x, y, father, font, text, dragable, name)
{
}



void InputText::HandleInput() {

	if (focused == true && App->input->inputTexActivated == false)
	{
		App->input->ActivateTextInput();
	}

	else if (focused == true)
	{
		if (previousText != App->input->getInputText())
		{
			string = App->input->getInputText();
			texture = App->font->Print(string.GetString(), { 200 }, font_Tex);

			int w, h;
			App->font->CalcSize(string.GetString(), w, h, font_Tex);

			my_box = new SDL_Rect{ position.x, position.y, w, h };
		}
		
	}

	else
	{
		App->input->DesactivateTextInput();
	}

}