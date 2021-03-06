#include "j1App.h"
#include "Button.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Input.h"


Button::Button(int x, int y, UIElement* father, j1Module* listener, SDL_Rect* buttonIdle, SDL_Rect* buttonSelected, SDL_Rect* buttonPressed, bool dragable, p2SString &name) : 

	UIElement(x, y, father, dragable, UI_type::BUTTON, name),
	button_idle(buttonIdle),
	button_selected(buttonSelected),
	button_pressed(buttonPressed),
	listener(listener)
{

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


Button::~Button() 
{
	delete button_idle;
	delete button_pressed;
	delete button_selected;

	button_idle = nullptr;
	button_pressed = nullptr;
	button_selected = nullptr;

	listener = nullptr;
}


bool Button::Start() {

	texture = App->gui->GetAtlas();

	my_box = new SDL_Rect{ position.x, position.y, button_idle->w, button_idle->h };

	started = true;

	return true;
}


bool Button::Draw() {

	if (pressed == true)
	{
		App->render->Blit(texture, position.x, position.y, button_pressed);
	}


	else if (focused == true)
	{
		App->render->Blit(texture, position.x, position.y, button_selected);
	}

	else
		App->render->Blit(texture, position.x, position.y, button_idle);

	return true;
}

void Button::Speaker(j1Module * listener) {

	if (listener != nullptr)
	{
		listener->ListenerUI(this);
	}
	
}


void Button::HandleInput() {

	if (focused == true && (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT))
	{
		pressed = true;
	}

	else if (focused == true && pressed == true && (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP))
	{
		Speaker(listener);
	}

	else
		pressed = false;
}