#include "UIElement.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1Input.h"

UIElement::UIElement()
{
}


UIElement::~UIElement()
{
}


bool UIElement::Start() {
	return true;
}


bool UIElement::Update() {

	if (father == nullptr)
	{
		position.x = local_position.x;
		position.y = local_position.y;
	}

	else
	{
		position.x = local_position.x + father->position.x;
		position.y = local_position.y + father->position.y;
	}

	return true;
}

bool UIElement::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {

		show_my_box = !show_my_box;
	}

	if (show_my_box == true && this->my_box != nullptr) {

		App->render->DrawQuad(*my_box, 0, 0, 150, 150);
	}
	return true;
}

bool UIElement::Draw() {
	return true;
}


UIElement* UIElement::GetFather() {
	return father;
}


bool UIElement::MouseUnderElement(int x, int y) {

	if (!(position.x + my_box->w < x || position.y + my_box->h < y || x < position.x || y < position.y))
	{
		focused = true;
	}
	else
		focused = false;

	return focused;
}


void UIElement::Drag(int x, int y) {

	local_position.x += x;
	local_position.y += y;

}

