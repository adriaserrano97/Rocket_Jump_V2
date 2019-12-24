#include "j1App.h"
#include "Window.h"
#include "j1Render.h"
#include "j1Gui.h"


Window::Window(int x, int y, UIElement* father, SDL_Rect* rect, bool dragable) :
	
	UIElement(x, y, father, dragable, UI_type::WINDOW),
	rect(rect)
{
	texture = nullptr;

	started = false;
	to_delete = false;

	focused = false;

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

Window::~Window() {

	father = nullptr;
	texture = nullptr;

	delete rect;
	rect = nullptr;
}



bool Window::Start() {

	texture = App->gui->GetAtlas();

	my_box = new SDL_Rect{ position.x, position.y, rect->w , rect->h };

	started = true;

	return true;
}


bool Window::Update() {

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


	return true;
}


bool Window::Draw() {

	App->render->Blit(texture, position.x, position.y, rect);

	return true;
}

