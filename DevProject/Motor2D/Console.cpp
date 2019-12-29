#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1FadeToBlack.h"
#include "Console.h"

#include "Brofiler/Brofiler/Brofiler.h"

j1Console::j1Console() : j1Module() {
	
	for (int i = 0; i < CONSOLE_ELEMENTS; i++)
	{
		consoleElements[i] = nullptr;
	}

	for (int i = 0; i < MAX_CONSOLE_TEXTS; i++)
	{
		consoleText[i] = nullptr;
	}
}


j1Console::~j1Console()
{
}

bool j1Console::PreUpdate() {
	BROFILER_CATEGORY("CONSOLE PREUPDATE", Profiler::Color::RosyBrown)
	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN && consoleOpen == false)
	{
		CreateConsole();
	}

	else if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN && consoleOpen == true)
	{
		DestroyConsole();
	}


	return true;
}


bool j1Console::Update() {
	BROFILER_CATEGORY("CONSOLE UPDATE", Profiler::Color::PaleTurquoise)

	return true;
}


bool j1Console::PostUpdate() {

	BROFILER_CATEGORY("CONSOLE POSTUPDATE", Profiler::Color::WhiteSmoke)
	return true;
}


void j1Console::CreateConsole() {
	BROFILER_CATEGORY("CONSOLE CREATION", Profiler::Color::LawnGreen)

	consoleElements[0] = App->gui->CreateUIWindow(0, 0, nullptr, new SDL_Rect{ 206, 0, 668, 364 }, false, p2SString("ConsoleWindow"));
	consoleElements[1] = App->gui->CreateUIWindow(650, 0, nullptr, new SDL_Rect{ 206, 0, 668, 364 }, false, p2SString("ConsoleWindow"));
	consoleElements[2] = App->gui->CreateUIWindow(0, 300, nullptr, new SDL_Rect{ 2, 621, 220, 17 }, false, p2SString("ConsoleWindow"));
	consoleElements[3] = App->gui->CreateUIWindow(218, 300, nullptr, new SDL_Rect{ 2, 621, 220, 17 }, false, p2SString("ConsoleWindow"));
	consoleElements[4] = App->gui->CreateInputText(20, 320, nullptr, App->font->default, p2SString("Write Here"), false, p2SString("ConsoleInpuText"));
	
	App->gui->RemoveAllFocus();
	consoleElements[4]->focused = true;
	consoleOpen = true;
}

void j1Console::DestroyConsole() {
	BROFILER_CATEGORY("CONSOLE DESTRUCTION", Profiler::Color::RoyalBlue)

	for (int i = 0; i < CONSOLE_ELEMENTS; i++)
	{
		if (consoleElements[i] != nullptr)
		{
			App->gui->DeleteElement(consoleElements[i]);
			consoleElements[i] = nullptr;
		}
	}

	consoleOpen = false;
}

void j1Console::ListenerUI(UIElement* UI_element) {

	p2SString aux = UI_element->name;

	aux.Cut(3, 12);

	if (UI_element->name == ("list"))
	{
		//LOG("list, god_mode, quit, fps<number>, map<number>");
		LOG("list, god_mode, quit, fps<number>, map<number>");
	}

	else if (UI_element->name == ("god_mode"))
	{
		
		
	}

	else if (aux == ("fps"))
	{
		int i;

		UI_element->name.Cut(0, UI_element->name.Find("<")+2);
		UI_element->name.Cut(UI_element->name.Find(">"), 15);
		sscanf_s(UI_element->name.GetString(), "%d", &i);

		if (i > 29 && i < 99)
		{
			App->frameRate = i;
		}
		else
		{
			LOG("Framerate %i is not avaliable", i);
		}
	}


	else if (aux == ("map"))
	{
		if (UI_element->name == "map<1>")
		{
			LOG("LOADING_MAP_1");
			
			App->scene->scene_number = 1;
			App->scene->ClearUIArray();
			App->map->Unload();
			App->fade->FadeToBlack(App->scene, App->scene, 2);
		}

		else if (UI_element->name == "map<2>")
		{
			LOG("LOADING_MAP_2");

			App->scene->scene_number = 2;
			App->map->Unload();
			App->fade->FadeToBlack(App->scene, App->scene, 2);
		}

		else if (UI_element->name == "map<3>")
		{
			LOG("LOADING_MAP_3");

			App->scene->scene_number = 3;
			App->map->Unload();
			App->fade->FadeToBlack(App->scene, App->scene, 2);
		}

		else
		{
			LOG("That map doesnt exist");
		}
	}

	else
	{
		LOG("Command doestn't exist");
	}
}


void j1Console::AddTextToConsole(p2SString &string) {

	for (int i = 0; i < MAX_CONSOLE_TEXTS; i++)
	{
		if (consoleText[i] == nullptr)
		{
			consoleText[i] = App->gui->CreateText(10, 20 * i + 10, nullptr, App->font->default, string, false, p2SString("ConsoleText"));
			return;
		}
	}

	App->gui->DeleteElement(consoleText[0]);

	for (size_t i = 0; i < MAX_CONSOLE_TEXTS-1; i++)
	{
		consoleText[i] = consoleText[i + 1];
		consoleText[i]->local_position.y = 20 * i + 10;
	}
	
	consoleText[9] = App->gui->CreateText(10, 20 * 9 + 10, nullptr, App->font->default, string, false, p2SString("ConsoleText"));
}