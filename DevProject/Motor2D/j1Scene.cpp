#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1FadeToBlack.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "j1EntityManager.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "Brofiler/Brofiler/Brofiler.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");

	memset(uiElements, NULL, 15);
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	BROFILER_CATEGORY("Scene Awake", Profiler::Color::DarkRed)
	LOG("Loading Scene");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	map1 = (config.child("map1").attribute("path").as_string());
	map2 = (config.child("map2").attribute("path").as_string());
	intro_menu = (config.child("intro_menu").attribute("path").as_string());

	//Saving valve
	load_from_save = false;
	load_lifes_from_save = false;
	MainMenu = false;
	CloseGameFromMenu = false;
	credits = false;
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	BROFILER_CATEGORY("Scene Start", Profiler::Color::DarkSalmon)
	//Our different maps. We only load the one we're currently using.
	map1;
	map2;
	intro_menu;
	playerNoLifes = false;

	switch (scene_number) {

	case 1:
		//scene_number = 1;
		App->gui->DestroyHUD();
		App->map->Load(map1.GetString());
		App->gui->CreateHUD();
		App->audio->PlayMusic("audio/music/ace_of_flopdisks.ogg", 4.0F);
		break;

	case 2:
		//scene_number = 2;
		App->gui->DestroyHUD();
		App->map->Load(map2.GetString());
		App->gui->CreateHUD();
		App->audio->PlayMusic("audio/music/down_under_flopdisk.ogg", 4.0F);
		break;

	case 3:
		//scene_number = 3;
		App->gui->DestroyHUD();
		LoadIntroMenu();
		load_from_save = false;
		load_lifes_from_save = false;
		
		App->audio->PlayMusic("audio/music/elevator_music.ogg", 4.0F);
		break;

	default:
		scene_number = 3;
		App->gui->DestroyHUD();
		LoadIntroMenu();
		load_from_save = false;
		load_lifes_from_save = false;
		
		App->audio->PlayMusic("audio/music/elevator_music.ogg", 4.0F);
		break;

	}

	debug_tex = App->tex->Load("maps/path2.png");
	
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	BROFILER_CATEGORY("Scene Pre-update", Profiler::Color::Tomato)

	bool ret = true;

	if (CloseGameFromMenu)
	{
		ret = false;
		ClearUIArray();
	}

	return ret;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::MintCream)
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		App->entityManager->Destroy_all();
		load_from_save = true;
		load_lifes_from_save = true;
		App->LoadGame();
	}
		
	
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame();

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 10;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 10;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 10;


	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 10;

	App->map->Draw();


	//Change scene logic
	if ((App->input->GetKey(SDL_SCANCODE_F1)) == KEY_DOWN) {
		scene_number = 1;
		App->map->Unload();
		App->fade->FadeToBlack(this, this, 2);
	}

	if ((App->input->GetKey(SDL_SCANCODE_F2)) == KEY_DOWN) {
		scene_number = 2;
		App->map->Unload();
		App->fade->FadeToBlack(this, this, 2);
	}


	if ((App->input->GetKey(SDL_SCANCODE_F3)) == KEY_DOWN) {
		scene_number = 3;
		App->map->Unload();
		App->fade->FadeToBlack(this, this, 2);
	}

	if ((App->input->GetKey(SDL_SCANCODE_6)) == KEY_DOWN) {
		App->gui->PlayerCoinsCounter++;
		App->gui->PlayerScoreCounter += 10;
		App->audio->PlayFx(App->audio->coin);
		App->gui->UpdateLifesNCoins();
	}
	if ((App->input->GetKey(SDL_SCANCODE_7)) == KEY_DOWN) {
		App->gui->PlayerLifesCounter++;
		App->gui->PlayerScoreCounter += 15;
		App->gui->UpdateLifesNCoins();
		App->audio->PlayFx(App->audio->powerup);
	}
	if (playerNoLifes) { PlayerNoLifes(); }



	// Debug pathfinding ------------------------------
	
	/*const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->PosConverter(path->At(i)->x, path->At(i)->y);
		App->render->Blit(debug_tex, pos.x, pos.y);
	}*/
	

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate() {
	BROFILER_CATEGORY("Scene Post-Update", Profiler::Color::MediumSlateBlue)
		bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		ret = false;
	}
	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	App->entityManager->Destroy_all();
	LOG("Freeing scene");
	return true;
}

void j1Scene::OnCollision(Collider* c1, Collider* c2) {

	if (c1->type == COLLIDER_END_LEVEL && c2->type == COLLIDER_PLAYER)
	{
		
		App->entityManager->godMode = true;
		
		if (scene_number == 1)
		{
			scene_number = 2;
			
			App->map->Unload();
			App->fade->FadeToBlack(this, this, 2);
			
		}

		else if (scene_number == 2)
		{
			scene_number = 1;
		
			App->map->Unload();
			App->fade->FadeToBlack(this, this, 2);
			
		}
	}
}

void j1Scene::ListenerUI(UIElement * UI_element)
{
	if (UI_element->name == "PLAY") {
		App->audio->PlayFx(App->audio->button_3);
		ClearUIArray();
		scene_number = 1;
		App->map->Unload();
		App->fade->FadeToBlack(this, this, 2);
	}

	if (UI_element->name == "CONTINUE") {
		App->audio->PlayFx(App->audio->button_3);
		ClearUIArray();
		App->entityManager->Destroy_all();
		load_from_save = true;
		load_lifes_from_save = true;
		
		App->LoadGame();
	}
	if (UI_element->name == "EXITGAME") {
		CloseGameFromMenu = true;
	}
	if (UI_element->name == "BACKTOMAIN") {
		App->audio->PlayFx(App->audio->button_3);
		ClearUIArray();
		App->gui->DestroyInGameMenu();
		scene_number = 3;
		App->map->Unload();
		App->fade->FadeToBlack(this, this, 2);
	}

	if (UI_element->name == "WEB") {
		App->audio->PlayFx(App->audio->button_3);
		ShellExecuteA(NULL,"open","https://adriaserrano97.github.io/Rocket_Jump_V2/",NULL,NULL,SW_SHOWNORMAL);
	}
	if (UI_element->name == "CREDITS") {
		App->audio->PlayFx(App->audio->button_3);
		DisplayCredits();
	}
}

bool j1Scene::LoadIntroMenu()
{
	//Clear the array before filling it
	ClearUIArray();
	//Load background
	//App->tex->Load(menu_background.GetString());
	
	App->map->Load(intro_menu.GetString());
	//Activate flag to render the menu
	MainMenu = true;



	//create UI
	//SDL_Rect* rect = new SDL_Rect{ 208, 0, 570, 363 };
	UIElement* principalWindow = App->gui->CreateUIWindow(100, 100, nullptr, new SDL_Rect{ 208, 0, 0, 0 }, false, p2SString("IntroWindow"));
	uiElements[0] = principalWindow;

	uiElements[1] = App->gui->CreateButton(310, 150, principalWindow, App->scene, new SDL_Rect{ 1040,413,207,49 }, new SDL_Rect{ 1040,1351,207,49 }, new SDL_Rect{ 1040,2289,207,49 }, false, p2SString("PLAY"));
	uiElements[2] = App->gui->CreateButton(310, 230, principalWindow, App->scene, new SDL_Rect{ 1040,483,207,49 }, new SDL_Rect{ 1040,1407,207,49 }, new SDL_Rect{ 1040,2361,207,49 }, false, p2SString("CONTINUE"));
	uiElements[3] = App->gui->CreateButton(360, 310, principalWindow, App->gui, new SDL_Rect{ 1120,205,113,36 }, new SDL_Rect{ 1120,1142,113,36 }, new SDL_Rect{ 1120,2087,113,36 }, false, p2SString("SETTINGS"));
	uiElements[4] = App->gui->CreateButton(360, 390, principalWindow, App->scene, new SDL_Rect{ 1120,309,113,36 }, new SDL_Rect{ 1120,1038,113,36 }, new SDL_Rect{ 1120,1975,113,36 }, false, p2SString("EXITGAME"));
	uiElements[5] = App->gui->CreateButton(360, 470, principalWindow, App->scene, new SDL_Rect{ 1120,347,113,36 }, new SDL_Rect{ 1120,1076,113,36 }, new SDL_Rect{ 1120,2013,113,36 }, false, p2SString("CREDITS"));
	uiElements[6] = App->gui->CreateButton(385, 540, principalWindow, App->scene, new SDL_Rect{ 608,413,60,55 }, new SDL_Rect{ 608,1351,60,55 }, new SDL_Rect{ 608,2288,60,55 }, false, p2SString("WEB"));
	return true;
}

void j1Scene::DisplayCredits()
{
	if (!credits) {
		credits = true;
		UIElement* creditWIndows = App->gui->CreateUIWindow(600, 550, nullptr, new SDL_Rect{ 738, 413, 204, 48 }, false, p2SString("creditWIndows"));
		uiElements[7] = creditWIndows;
		UIElement* CreditsText = App->gui->CreateText(5, 0, creditWIndows, App->font->default, p2SString("License: Simple DirectMedia Layer"), false, p2SString("CreditsText"));
		uiElements[8] = CreditsText;
		UIElement* CreditsText2 = App->gui->CreateText(5, 9, creditWIndows, App->font->default, p2SString("Done by Adria Serrano"), false, p2SString("CreditsText2"));
		uiElements[9] = CreditsText2;
		UIElement* CreditsText3 = App->gui->CreateText(5, 18, creditWIndows, App->font->default, p2SString("& Jose Luis Redondo"), false, p2SString("CreditsText3"));
		uiElements[10] = CreditsText3;
		UIElement* CreditsText4 = App->gui->CreateText(5, 27, creditWIndows, App->font->default, p2SString("Click on the star to know more!"), false, p2SString("CreditsText4"));
		uiElements[11] = CreditsText4;
		
	}
	else {
		credits = false;
		App->gui->DeleteElement(uiElements[7]);
		App->gui->DeleteElement(uiElements[8]);
		App->gui->DeleteElement(uiElements[9]);
		App->gui->DeleteElement(uiElements[10]);
		App->gui->DeleteElement(uiElements[11]);

		uiElements[7] = nullptr;
		uiElements[8] = nullptr;
		uiElements[9] = nullptr;
		uiElements[10] = nullptr;
		uiElements[11] = nullptr;
	
	}

}

void j1Scene::PlayerNoLifes()
{
	scene_number = 3;
	App->entityManager->Destroy_all();
	App->map->Unload();
	App->gui->PlayerLifesCounter = 1; App->gui->PlayerCoinsCounter = 0; App->gui->PlayerScoreCounter = 0;
	App->fade->FadeToBlack(this, this, 2);
}

void j1Scene::ClearUIArray()
{
	for (int i = 0; i < 15; i++)
	{
		if (uiElements[i] != nullptr)
		{
			App->gui->DeleteElement(uiElements[i]);
			uiElements[i] = nullptr;
		}
	}
}

bool j1Scene::AddUIElementToArray(UIElement * element)
{
	bool ret = false;
	for (int i = 0; i < 15; i++)
	{
		if (uiElements[i] != nullptr)
		{
			continue;
		}
		else {
			uiElements[i] = element;
			ret = true;
			break;
		}
	}
	return ret;
}

int j1Scene::FindElementinArray(UIElement * element)
{
	int ret = -1;
	for (int i = 0; i < 15; i++)
	{
		if (uiElements[i] != element)
		{
			continue;
		}
		else {
			ret = i;
			break;
		}
	}
	return ret;
}

int j1Scene::FindElementinArrayName(p2SString * name)
{
	int ret = -1;
	for (int i = 0; i < 15; i++)
	{
		if (uiElements[i]->name != *name)
		{
			continue;
		}
		else {
			ret = i;
			break;
		}
	}
	return ret;
}

bool j1Scene::Load(pugi::xml_node& data) {
	
	if (scene_number != data.attribute("sceneNumber").as_int())
	{
		scene_number = data.attribute("sceneNumber").as_int();
		
		App->map->Unload();
		App->fade->FadeToBlack(this, this, 2);
	}
	else
	{
		App->fade->FadeToBlack(2);
	}
	

	return true;
}

// Save Game State
bool j1Scene::Save(pugi::xml_node& data) const {

	data.append_attribute("sceneNumber") = scene_number;

	return true;
}