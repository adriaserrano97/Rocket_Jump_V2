#include "UIElement.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Gui.h"

UIElement::UIElement()
{
}


UIElement::~UIElement()
{
	father = nullptr;
	texture = nullptr;

	delete my_box;
	my_box = nullptr;

	name.Clear();
}


UIElement::UIElement(int x, int y, UIElement* father, bool dragable, UI_type type, p2SString &name) :
	position(x, y),
	local_position(x, y),
	father(father),
	dragable(dragable),
	type(type),
	name(name),
	focused(false),
	started(false),
	to_delete(false),
	show_my_box(false),
	texture(nullptr),
	my_box(nullptr)
{
}


bool UIElement::Start() {
	return true;
}


bool UIElement::Update() {

	Move();
	HandleInput();

	return true;
}


void UIElement::Move() {

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

}


void UIElement::HandleInput() {
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
		/*if (focused == false) {
			App->gui->RemoveAllFocus();
			focused = true;
		}*/
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


bool UIElement::operator==(UIElement* element) {
	
	if (position == element->position && local_position == element->local_position && type == element->type && texture == element->texture && father == element->father && dragable == element->dragable)
	{
		return true;
	}

	return false;

}

