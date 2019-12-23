#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "Text.h"
#include "Button.h"
#include "UIElement.h"
#include "Window.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");

	for (int i = 0; i <= MAX_ELEMENTS; i++)
	{
		elementArray[i] = nullptr;
	}

	elementDragged = nullptr;

}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlasFileName = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlasFileName.GetString());

	CreateGUIElement(UI_type::TEXT, 50, 50, nullptr, "YEEEEEE BOY");

	CreateGUIElement(UI_type::WINDOW, 50, 50, nullptr);

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] != nullptr)
		{
			if (elementArray[i]->started == false)
			{
				elementArray[i]->Start();
			}

			CheckFocusedElements();
			DragElement();
			elementArray[i]->Update();
		}
	}
	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] != nullptr)
		{
			elementArray[i]->PostUpdate();
			elementArray[i]->Draw();
		}

	}
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	return true;
}

// const getter for atlas
SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

void j1Gui::ListenerUI(UIElement * UI_element)
{
	/*check what UI_element are you getting and do whatevs you want.

	if(UI_element.name == "button to change scene"){delete this button;}

	etc.
	*/
}

void j1Gui::CreateGUIElement(UI_type type, int x, int y, UIElement* father, p2SString text) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == nullptr)
		{
			switch (type)
			{
			case UI_type::FAIL:
				LOG("Incorrect element, that type doesnt exist");
				break;
			case UI_type::BUTTON:
				elementArray[i] = new Button(x, y, father);
				break;
			case UI_type::TEXT:
				elementArray[i] = new Text(x, y, text, father);
				break;

			case UI_type::WINDOW:
				elementArray[i] = new Window(x, y, father);
				break;

			case UI_type::STATIC_IMAGE:
				break;
			case UI_type::MAX:
				break;
			default:
				break;
			}

			break;
		}
	}

}


void j1Gui::CheckFocusedElements() {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] != nullptr)
		{
			elementArray[i]->focused = false;
		}
	}


	iPoint pos;
	App->input->GetMousePosition(pos.x, pos.y);
	pos = App->render->ScreenToWorld(pos.x, pos.y);

	for (int i = MAX_ELEMENTS - 1; i >= 0; i--)
	{
		if (elementArray[i] != nullptr && elementArray[i]->my_box != nullptr)
		{
			if (elementArray[i]->MouseUnderElement(pos.x, pos.y))
			{
				break;
			}

		}
	}

}


void j1Gui::DragElement() {

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		App->input->GetMousePosition(mouseDragX, mouseDragY);

		for (int i = 0; i < MAX_ELEMENTS; i++)
		{
			if (elementArray[i] != nullptr)
			{
				if (elementArray[i]->focused == true && elementArray[i]->dragable == true)
				{
					elementDragged = elementArray[i];
					break;
				}

			}
		}
	}


	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (elementDragged != nullptr)
		{
			int x, y;

			App->input->GetMousePosition(x, y);

			elementDragged->Drag(x - mouseDragX, y - mouseDragY);

			mouseDragX = x;
			mouseDragY = y;
		}
	}


	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		elementDragged = nullptr;
	}
}

