#include "j1App.h"
#include "Window.h"
#include "j1Render.h"
#include "j1Gui.h"

Window::Window(int x, int y, UIElement* father) {

	texture = nullptr;

	started = false;
	to_delete = false;
	dragable = true;
	focused = false;

	this->father = father;

	position.x = x;
	position.y = y;


	rect = nullptr;

	type = UI_type::WINDOW;
}


Window::~Window() {

	father = nullptr;
	texture = nullptr;

	delete rect;
	rect = nullptr;
}



bool Window::Start() {

	texture = App->gui->GetAtlas();

	rect = new SDL_Rect{ 32, 543, 419, 449 };

	my_box = new SDL_Rect{ position.x, position.y, rect->w , rect->h };

	App->gui->CreateGUIElement(UI_type::BUTTON, 150, 150, this);

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

