#include "j1App.h"
#include "InputText.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Textures.h"7
#include "Console.h"


InputText::InputText(int x, int y, UIElement* father, _TTF_Font* font, p2SString& text, bool dragable, p2SString& name) :
	Text(x, y, father, font, text, dragable, name)
{

	cursorPos = text.Length();
}


InputText::~InputText() {

	previousText.Clear();
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
			string = App->input->getInputText();

			App->tex->UnLoad(texture);
			texture = App->font->Print(string.GetString());

			previousText = string;
		}
	}

	else
	{
		App->input->DesactivateTextInput();
	}

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		name = string;
		Speak();
		string.Clear();
		App->input->DesactivateTextInput();
	}
	
}


bool InputText::Draw() {

	App->render->Blit(texture, position.x, position.y);

	return true;
}


void InputText::Speak() {

	if (App->console)
	{
		App->console->ListenerUI(this);
	}

}