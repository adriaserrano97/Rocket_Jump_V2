#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "UIElement.h"
#include "Text.h"
#include "Button.h"
#include "Window.h"
#include "ScrollBar.h"
#include "InputText.h"
#include "j1Audio.h"
#include "Brofiler/Brofiler/Brofiler.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");

	for (int i = 0; i <= MAX_ELEMENTS; i++)
	{
		elementArray[i] = nullptr;
	}
	
	memset(InGameMenuUIElements, NULL, 15);
	inGameMenu = false;

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
	PlayerLifesCounter = 1;
	PlayerCoinsCounter = 0;
	PlayerScoreCounter = 0;

	atlasFileName = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	BROFILER_CATEGORY("GUI Start", Profiler::Color::DarkKhaki)
	atlas = App->tex->Load(atlasFileName.GetString());

	/*SDL_Rect* rect = new SDL_Rect{ 32, 543, 419, 449 };
	CreateUIWindow(50, 50, nullptr, rect, true);

	CreateScrollBar(80, 80, nullptr, nullptr, new SDL_Rect{136, 600, 30, 120}, new SDL_Rect{ 842, 328, 16, 13 }, new SDL_Rect{ 1003, 437, 16, 13 }, true);
	
	j1Module* listeners[10];
	memset(listeners, NULL, 10);
	
	CreateButton(120, 100, nullptr, listeners, new SDL_Rect{ 642,169,229,69 }, new SDL_Rect{ 0,113,229,69 }, new SDL_Rect{ 411,169,229,69 }, true);
	*/
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

	ManageInGameMenu();
	ManageHUD();


	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] != nullptr)
		{
			delete  elementArray[i];
			elementArray[i] = nullptr;
		}

	}
	return true;
}


void j1Gui::DeleteElement(UIElement* element) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == element)
		{
			delete elementArray[i];
			elementArray[i] = nullptr;
			return;
		}
	}

}


// const getter for atlas
SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

void j1Gui::ListenerUI(UIElement * element)
{
	if (element->name == "RETRY") {

		DestroyInGameMenu();
	}

	else if (element->name == "SETTINGS")
	{
		CreateSettingsWindow(element);
	}
	
	else if (element->name == "CROSS")
	{
		DestroySettingsWindow();
	}

}

void j1Gui::RemoveAllFocus()
{
	if (!inGameMenu) {
		for (int i = 0; i < MAX_ELEMENTS; i++)
		{
			if (elementArray[i] != NULL && elementArray[i]->focused == true)
			{
				elementArray[i]->focused = false;
			}
		}
	}
	if (inGameMenu) {
		for (int i = 0; i < 15; i++)
		{
			if (InGameMenuUIElements[i] != NULL && InGameMenuUIElements[i]->focused == true)
			{
				InGameMenuUIElements[i]->focused = false;
			}
		}
	}
}


UIElement* j1Gui::CreateButton(int x, int y, UIElement* father, j1Module* listener, SDL_Rect* buttonIdle, SDL_Rect* buttonSelected, SDL_Rect* buttonPressed, bool dragable, p2SString& name) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == nullptr) {

			elementArray[i] = new Button(x, y, father, listener, buttonIdle, buttonSelected, buttonPressed, dragable, name);
				
			return elementArray[i];
		}
	}
}


UIElement* j1Gui::CreateUIWindow(int x, int y, UIElement* father, SDL_Rect* rect, bool dragable, p2SString& name) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == nullptr) {

			elementArray[i] = new Window(x, y, father, rect, dragable, name);

			return elementArray[i];
		}
	}
}


UIElement* j1Gui::CreateText(int x, int y, UIElement* father, _TTF_Font* font, p2SString &text, bool dragable, p2SString& name) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == nullptr) {

			elementArray[i] = new Text(x, y, father, font, text, dragable, name);

			return elementArray[i];
		}
	}
}


UIElement* j1Gui::CreateInputText(int x, int y, UIElement* father, _TTF_Font* font, p2SString& text, bool dragable, p2SString& name) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == nullptr) {

			elementArray[i] = new InputText(x, y, father, font, text, dragable, name);

			return elementArray[i];
		}
	}
}


UIElement* j1Gui::CreateScrollBar(int x, int y, UIElement* father, j1Module* listener, SDL_Rect* bar, SDL_Rect* thumbsIdle, SDL_Rect* thumbsPressed, bool vertical, p2SString& name) {

	for (int i = 0; i < MAX_ELEMENTS; i++)
	{
		if (elementArray[i] == nullptr) {

			elementArray[i] = new Window(x, y, father, bar, false, name);
			elementArray[i + 1] = new ScrollBar(0, 0, elementArray[i], listener, thumbsIdle, thumbsPressed, vertical, name);

			return elementArray[i+1];
		}
	}
}


void j1Gui::CheckFocusedElements() {

	if (!inGameMenu) {
		/*
		for (int i = 0; i < MAX_ELEMENTS; i++)
		{
			if (elementArray[i] != nullptr)
			{
				elementArray[i]->focused = false;
			}
		}
		*/
		iPoint pos;
		App->input->GetMousePosition(pos.x, pos.y);
		pos = App->render->ScreenToWorld(pos.x, pos.y);

		for (int i = MAX_ELEMENTS - 1; i >= 0; i--)
		{
			if (elementArray[i] != nullptr && elementArray[i]->my_box != nullptr && elementArray[i]->type != UI_type::WINDOW)
			{
				//if (elementArray[i]->MouseUnderElement(pos.x, pos.y))
				//{
					//break;
				//}
				elementArray[i]->MouseUnderElement(pos.x, pos.y);

			}
		}
	}
	if (inGameMenu) {
		iPoint pos;
		App->input->GetMousePosition(pos.x, pos.y);
		pos = App->render->ScreenToWorld(pos.x, pos.y);

		for (int i = 15 - 1; i >= 0; i--)
		{
			if (InGameMenuUIElements[i] != nullptr && InGameMenuUIElements[i]->my_box != nullptr && InGameMenuUIElements[i]->type != UI_type::WINDOW)
			{
				InGameMenuUIElements[i]->MouseUnderElement(pos.x, pos.y);
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


void j1Gui::ManageInGameMenu() {

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && inGameMenu == false) {

		CreateInGameMenu();
		
	}

	else if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && inGameMenu == true) {

		DestroyInGameMenu();
		
	}
}


void j1Gui::CreateInGameMenu() {
	
	UIElement* principalWindow = App->gui->CreateUIWindow(50, 50, nullptr, new SDL_Rect{ 206, 0, 668, 364 }, true, p2SString("InGameWindow"));

	InGameMenuUIElements[0] = principalWindow;

	InGameMenuUIElements[1] = App->gui->CreateButton(120, 100, principalWindow, this, new SDL_Rect{ 955,413,80,36 }, new SDL_Rect{ 955,1350,80,36 }, new SDL_Rect{ 955,2288,80,36 }, false, p2SString("RETRY"));

	InGameMenuUIElements[2] = App->gui->CreateButton(120, 150, principalWindow, this, new SDL_Rect{ 1120,205,113,36 }, new SDL_Rect{ 1120,1142,113,36 }, new SDL_Rect{ 1120,2087,113,36 }, false, p2SString("SETTINGS"));
	InGameMenuUIElements[3] = App->gui->CreateButton(120, 200, principalWindow, App->scene, new SDL_Rect{ 1120,309,113,36 }, new SDL_Rect{ 1120,1038,113,36 }, new SDL_Rect{ 1120,1975,113,36 }, false, p2SString("EXITGAME"));
	InGameMenuUIElements[4] = App->gui->CreateButton(120, 250, principalWindow, nullptr, new SDL_Rect{ 608,413,60,55 }, new SDL_Rect{ 608,1351,60,55 }, new SDL_Rect{ 608,2288,60,55 }, false, p2SString("CREDITS"));

	inGameMenu = true;
}


void j1Gui::DestroyInGameMenu() {

	for (int i = 0; i < 15; i++)
	{
		if (InGameMenuUIElements[i] != nullptr)
		{
			App->gui->DeleteElement(InGameMenuUIElements[i]);
			InGameMenuUIElements[i] = nullptr;
		}
	}

	inGameMenu = false;
}

void j1Gui::ManageHUD() {

	if (PlayerCoinsCounter >= 5) {
		PlayerCoinsCounter -= 5;
		PlayerLifesCounter += 1;
		PlayerScoreCounter += 15;
		UpdateLifesNCoins();
		App->audio->PlayFx(App->audio->powerup);
	}
	
	/*
	timer?
	
	*/
}

void j1Gui::CreateHUD() {

	CoinsWindow = App->gui->CreateUIWindow(50, 50, nullptr, new SDL_Rect{ 676, 1350, 49, 26 }, false, p2SString("CoinsWindow"));
	HUDUIElements[0] = CoinsWindow;

	LifesWindow = App->gui->CreateUIWindow(120, 50, nullptr, new SDL_Rect{ 676, 1381, 49, 26 }, false, p2SString("LifesWindow"));
	HUDUIElements[1] = LifesWindow;

	char coins_text[] = "fillingspace";
	sprintf_s(coins_text, 10, "%02d", PlayerCoinsCounter);
	UIElement* CoinsText = App->gui->CreateText(30, 8, CoinsWindow, App->font->default, p2SString(coins_text), false, p2SString("CoinsText"));
	HUDUIElements[2] = CoinsText;

	char lifes_text[] = "fillingspace";
	sprintf_s(lifes_text, 10, "%02d", PlayerLifesCounter);
	UIElement* LifesText = App->gui->CreateText(30, 8, LifesWindow, App->font->default, p2SString(lifes_text), false, p2SString("LifesText"));
	HUDUIElements[3] = LifesText;

	ScoreWindow = App->gui->CreateUIWindow(190, 50, nullptr, new SDL_Rect{ 676, 1410, 49, 26 }, false, p2SString("ScoreWindow"));
	HUDUIElements[4] = ScoreWindow;

	char score_text[] = "fillingspace";
	sprintf_s(score_text, 10, "%03d", PlayerScoreCounter);
	UIElement* ScoreText = App->gui->CreateText(25, 8, ScoreWindow, App->font->default, p2SString(score_text), false, p2SString("ScoreText"));
	HUDUIElements[5] = ScoreText;
	
}

void j1Gui::DestroyHUD() {
	
	for (int i = 0; i < 15; i++)
	{
		if (HUDUIElements[i] != nullptr)
		{
			App->gui->DeleteElement(HUDUIElements[i]);
			HUDUIElements[i] = nullptr;
		}
	}

}

void j1Gui::UpdateLifesNCoins()
{
	App->gui->DeleteElement(HUDUIElements[2]);
	HUDUIElements[2] = nullptr;

	App->gui->DeleteElement(HUDUIElements[3]);
	HUDUIElements[3] = nullptr;

	App->gui->DeleteElement(HUDUIElements[5]);
	HUDUIElements[5] = nullptr;


	char coins_text[] = "fillingspace";
	sprintf_s(coins_text, 10, "%02d", PlayerCoinsCounter);
	UIElement* CoinsText = App->gui->CreateText(30, 8, CoinsWindow, App->font->default, p2SString(coins_text), false, p2SString("CoinsText"));
	HUDUIElements[2] = CoinsText;

	char lifes_text[] = "fillingspace";
	sprintf_s(lifes_text, 10, "%02d", PlayerLifesCounter);
	UIElement* LifesText = App->gui->CreateText(30, 8, LifesWindow, App->font->default, p2SString(lifes_text), false, p2SString("LifesText"));
	HUDUIElements[3] = LifesText;

	char score_text[] = "fillingspace";
	sprintf_s(score_text, 10, "%03d", PlayerScoreCounter);
	UIElement* ScoreText = App->gui->CreateText(25, 8, ScoreWindow, App->font->default, p2SString(score_text), false, p2SString("ScoreText"));
	HUDUIElements[5] = ScoreText;
}

void j1Gui::CreateSettingsWindow(UIElement* father) {

	UIElement* settingsWindow = InGameMenuUIElements[5] = App->gui->CreateUIWindow(120, -70, father, new SDL_Rect{ 682, 620, 154, 192 }, false, p2SString("SettingsWindow"));

	InGameMenuUIElements[6] = App->gui->CreateButton(120, 5, settingsWindow, this, new SDL_Rect{ 892,620,32,30 }, new SDL_Rect{ 892,1557,32,30 }, new SDL_Rect{ 892,2495,32,30 }, false, p2SString("CROSS"));
	InGameMenuUIElements[7] = App->gui->CreateInputText(20, 60, settingsWindow, App->font->default, p2SString("Game volume"), false, p2SString("VolumeText"));
	InGameMenuUIElements[8] = App->gui->CreateScrollBar(20, 120, settingsWindow, App->audio, new SDL_Rect{ 2, 621, 120, 17 }, new SDL_Rect{ 226, 2709, 20, 22 }, new SDL_Rect{ 226, 1772, 20, 22 }, false, p2SString("HorizontalVolumeScrollBar"));
}


void j1Gui::DestroySettingsWindow() {

	DeleteElement(InGameMenuUIElements[5]);
	DeleteElement(InGameMenuUIElements[6]);
	DeleteElement(InGameMenuUIElements[7]);
	DeleteElement(InGameMenuUIElements[8]);

	InGameMenuUIElements[5] = nullptr;
	InGameMenuUIElements[6] = nullptr;
	InGameMenuUIElements[7] = nullptr;
	InGameMenuUIElements[8] = nullptr;
}

bool j1Gui::Save(pugi::xml_node & data) const
{
	pugi::xml_node coin = data.append_child("gui_counter");
	coin.append_attribute("type") = "coins";
	coin.append_attribute("value") = PlayerCoinsCounter;
	pugi::xml_node lifes = data.append_child("gui_counter");
	lifes.append_attribute("type") = "lifes";
	lifes.append_attribute("value") = PlayerLifesCounter;
	pugi::xml_node score = data.append_child("gui_counter");
	score.append_attribute("type") = "score";
	score.append_attribute("value") = PlayerScoreCounter;
	
	return true;
				
}

bool j1Gui::Load(pugi::xml_node & data)
{
	if (App->scene->load_lifes_from_save == true) {
		for (pugi::xml_node iterator = data.first_child(); iterator != NULL; iterator = iterator.next_sibling())
		{
			p2SString type(iterator.attribute("type").as_string());
			int value(iterator.attribute("value").as_int());

			if (type == "coins")
			{
				PlayerCoinsCounter = value;
			}

			if (type == "lifes")
			{
				PlayerLifesCounter = value;
			}

			if (type == "score")
			{
				PlayerScoreCounter = value;
			}
		}
	}
	App->scene->load_lifes_from_save = false;
	UpdateLifesNCoins();
	return true;
}
