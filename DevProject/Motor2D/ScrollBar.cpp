#include "j1App.h"
#include "ScrollBar.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Input.h"

ScrollBar::ScrollBar(int x, int y, UIElement* father, j1Module* listener, SDL_Rect* thumbsIdle, SDL_Rect* thumbsPressed, bool vertical, p2SString &name) :

	UIElement(x, y, father, true, UI_type::SCROLL_BAR, name),
	thumbIdle(thumbsIdle),
	thumbPressed(thumbsPressed),
	listener(listener),
	value(0),
	vertical(vertical)
{

	if (father == nullptr)
		assert("Must have a father");

	position += father->position;

	local_position = { x, y };
	
}


ScrollBar::~ScrollBar() {

	delete thumbIdle;
	delete thumbPressed;

	thumbIdle = nullptr;
	thumbPressed = nullptr;

	App->gui->DeleteElement(father);
}


bool ScrollBar::Start() {

	texture = App->gui->GetAtlas();

	my_box = new SDL_Rect{ position.x, position.y, thumbIdle->w, thumbIdle->h };

	started = true;

	return true;
}


void ScrollBar::Move() {

	if (vertical == true)
	{
		if (local_position.y > father->my_box->h - thumbIdle->h)
			local_position.y = father->my_box->h - thumbIdle->h;

		else if (local_position.y < 0)
			local_position.y = 0;

		position.y = local_position.y + father->position.y;
		position.x = father->position.x;
	}

	else
	{
		if (local_position.x > father->my_box->w - thumbIdle->w)
			local_position.x = father->my_box->w - thumbIdle->w;

		else if (local_position.x < 0)
			local_position.x = 0;

		position.x = local_position.x + father->position.x;
		position.y = father->position.y;
	}
	

}


void ScrollBar::HandleInput() {

	if (focused == true && (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)) {
		
		pressed = true;
		
		Speak();	
	}
	else
		pressed = false;
}


bool ScrollBar::Draw() {

	if (pressed == true)
	{
		App->render->Blit(texture, position.x, position.y, thumbPressed);
	}

	else
		App->render->Blit(texture, position.x, position.y, thumbIdle);

	return true;
}


void ScrollBar::Speak()
{
	listener->ListenerUI(this);
}