#include "j1App.h"
#include "Button.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Input.h"


Button::Button(int x, int y, UIElement* father) {

	texture = nullptr;

	button_idle = nullptr;
	button_pressed = nullptr;
	button_selected = nullptr;

	started = false;
	to_delete = false;

	dragable = true;
	focused = false;

	pressed = false;
	focused = false;

	this->father = father;

	if (father == nullptr)
	{
		position.x = x;
		position.y = y;
		local_position = { 0, 0 };
	}

	else
	{
		position.x = x + father->position.x;
		position.y = y + father->position.y;
		local_position = { x, y };
	}

	type = UI_type::BUTTON;
}

Button::Button(int x, int y, UIElement * father, j1Module* listeners[10])
{
	
	
	//llename pls
}


Button::~Button() {

	delete button_idle;
	delete button_pressed;
	delete button_selected;

	button_idle = nullptr;
	button_pressed = nullptr;
	button_selected = nullptr;

	father = nullptr;
	texture = nullptr;

}


bool Button::Start() {

	texture = App->gui->GetAtlas();

	button_idle = new SDL_Rect{ 0, 113, 229, 69 };
	button_pressed = new SDL_Rect{ 411,169,229, 69 };
	button_selected = new SDL_Rect{ 642,169,229, 69 };

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

void Button::Speaker(j1Module * listener)
{

	listener->ListenerUI(this);

}

void Button::SpeakToAll()
{
	uint i = 0;
	
	while(listeners[0] != nullptr){	

		listeners[i]->ListenerUI(this);
	
	}

}


bool Button::Update() {

	if (father == nullptr)
	{
		position.x = local_position.x - App->render->camera.x;
		position.y = local_position.y - App->render->camera.y;
	}

	else
	{
		position.x = local_position.x + father->position.x;
		position.y = local_position.y + father->position.y;
	}


	if (focused == true && (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT))
		pressed = true;
	else
		pressed = false;


	return true;
}