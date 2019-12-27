#include "j1App.h"
#include "Window.h"
#include "j1Render.h"
#include "j1Gui.h"


Window::Window(int x, int y, UIElement* father, SDL_Rect* rect, bool dragable, p2SString &name) :
	
	UIElement(x, y, father, dragable, UI_type::WINDOW, name),
	rect(rect)
{
	if (father != nullptr)
	{
		position.x += father->position.x;
		position.y += father->position.y;
	}
	

}

Window::~Window() {

	father = nullptr;
	texture = nullptr;

	delete rect;
	rect = nullptr;

	delete my_box;
	my_box = nullptr;
}



bool Window::Start() {

	texture = App->gui->GetAtlas();

	my_box = new SDL_Rect{ position.x, position.y, rect->w , rect->h };

	started = true;

	return true;
}


bool Window::Draw() {

	App->render->Blit(texture, position.x, position.y, rect);

	return true;
}

