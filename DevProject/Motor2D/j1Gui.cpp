#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "UIElement.h"
#include "Text.h"
#include "Button.h"
#include "Window.h"
#include "ScrollBar.h"
#include "Brofiler/Brofiler/Brofiler.h"

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
	BROFILER_CATEGORY("GUI Awake", Profiler::Color::DarkGreen)
	LOG("Loading GUI atlas");
	bool ret = true;

	atlasFileName = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	BROFILER_CATEGORY("GUI Start", Profiler::Color::DarkKhaki)
	atlas = App->tex->Load(atlasFileName.GetString());

	/*SDL_Rect* rect = new SDL_Rect{ 32, 543, 419, 449 };
	CreateUIWindow(50, 50, nullptr, rect, true);*/

	CreateScrollBar(80, 80, nullptr, nullptr, /*new SDL_Rect{ 975, 788, 6, 163 }*/new SDL_Rect{136, 600, 30, 120}, new SDL_Rect{ 842, 328, 16, 13 }, new SDL_Rect{ 1003, 437, 16, 13 });

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	BROFILER_CATEGORY("GUI pre-update", Profiler::Color::Black)

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
	BROFILER_CATEGORY("GUI post-update", Profiler::Color::BlanchedAlmond)
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


UIElement* j1Gui::CreateButton(int x, int y, UIElement* father, j1Module* listeners[10], SDL_Rect* buttonIdle, SDL_Rect* buttonSelected, SDL_Rect* buttonPressed, bool dragable) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == nullptr) {

			elementArray[i] = new Button(x, y, father, listeners, buttonIdle, buttonSelected, buttonPressed, dragable);
				
			return elementArray[i];
		}
	}
}


UIElement* j1Gui::CreateUIWindow(int x, int y, UIElement* father, SDL_Rect* rect, bool dragable) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == nullptr) {

			elementArray[i] = new Window(x, y, father, rect, dragable);

			return elementArray[i];
		}
	}
}


UIElement* j1Gui::CreateText(int x, int y, UIElement* father, _TTF_Font* font, p2SString &text, bool dragable) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == nullptr) {

			elementArray[i] = new Text(x, y, father, font, text, dragable);

			return elementArray[i];
		}
	}
}


UIElement* j1Gui::CreateScrollBar(int x, int y, UIElement* father, j1Module* listener, SDL_Rect* bar, SDL_Rect* thumbsIdle, SDL_Rect* thumbsPressed) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == nullptr) {

			elementArray[i] = new Window(x, y, father, bar, false);
			elementArray[i + 1] = new ScrollBar(0, 0, elementArray[i], nullptr, thumbsIdle, thumbsPressed);

			return elementArray[i+1];
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

