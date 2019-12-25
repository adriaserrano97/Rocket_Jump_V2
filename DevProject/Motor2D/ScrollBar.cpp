#include "j1App.h"
#include "ScrollBar.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1Audio.h"

ScrollBar::ScrollBar(int x, int y, UIElement* father, j1Module* listener, SDL_Rect* thumbsIdle, SDL_Rect* thumbsPressed) :

	UIElement(x, y, father, true, UI_type::SCROLL_BAR),
	thumbIdle(thumbsIdle),
	thumbPressed(thumbsPressed),
	value(0)
{

	texture = nullptr;

	started = false;
	to_delete = false;

	pressed = false;
	focused = false;

	if (father == nullptr)
		assert("Must have a father");

	position.x += father->position.x;
	position.y += father->position.y;
	local_position = { x, y };
	
}


ScrollBar::~ScrollBar() {

	delete thumbIdle;
	delete thumbPressed;


	thumbIdle = nullptr;
	thumbPressed = nullptr;

	father = nullptr;
	texture = nullptr;

}


bool ScrollBar::Start() {

	texture = App->gui->GetAtlas();

	my_box = new SDL_Rect{ position.x, position.y, thumbIdle->w, thumbIdle->h };

	started = true;

	return true;
}


void ScrollBar::Move() {

	if (local_position.y > father->my_box->h)
		local_position.y = father->my_box->h;

	if (local_position.y < 0)
		local_position.y = 0;

	position.y = local_position.y + father->position.y;
	position.x = father->position.x;

}


void ScrollBar::HandleInput() {

	if (focused == true && (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)) {
		pressed = true;

		value = -local_position.y * MAX_VOLUME / father->my_box->h;
		Mix_VolumeMusic(value + MAX_VOLUME);
		Mix_Volume(-1, value + MAX_VOLUME);
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