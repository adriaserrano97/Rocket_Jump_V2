#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
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

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		CreateConsole();
	}

	return true;
}


bool j1Console::Update() {


	return true;
}


bool j1Console::PostUpdate() {


	return true;
}


void j1Console::CreateConsole() {

	consoleElements[0] = App->gui->CreateUIWindow(0, 0, nullptr, new SDL_Rect{ 206, 0, 668, 364 }, false, p2SString("ConsoleWindow"));
	consoleElements[1] = App->gui->CreateUIWindow(650, 0, nullptr, new SDL_Rect{ 206, 0, 668, 364 }, false, p2SString("ConsoleWindow"));
	consoleElements[2] = App->gui->CreateUIWindow(0, 300, nullptr, new SDL_Rect{ 2, 621, 220, 17 }, false, p2SString("ConsoleWindow"));
	consoleElements[3] = App->gui->CreateUIWindow(218, 300, nullptr, new SDL_Rect{ 2, 621, 220, 17 }, false, p2SString("ConsoleWindow"));
	consoleElements[4] = App->gui->CreateText(20, 320, nullptr, App->font->default, p2SString("Write Here"), false, p2SString("ConsoleInpuText"));
}


void j1Console::ListenerUI(UIElement* UI_element) {

	if (UI_element->name == ("list"))
	{
		LOG("list, god_mode, quit, fps<number>, map<number>");
	}

	else if (UI_element->name == ("god_mode"))
	{

	}

	else if (UI_element->name == ("fps<number>"))
	{

	}
	

	else if (UI_element->name == ("map<number>"))
	{

	}
}