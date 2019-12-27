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
#include "Brofiler/Brofiler/Brofiler.h"

j1Scene::j1Scene() : j1Module()
{
	inGameMenu = false;
	name.create("scene");
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

	//Saving valve
	load_from_save = false;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	BROFILER_CATEGORY("Scene Start", Profiler::Color::DarkSalmon)
	//Our different maps. We only load the one we're currently using.
	map1;
	map2;
	
	switch (scene_number) {

	case 1:
		App->map->Load(map1.GetString());

		App->audio->PlayMusic("audio/music/ace_of_flopdisks.ogg", 4.0F);
		break;

	case 2:
		App->map->Load(map2.GetString());
		App->audio->PlayMusic("audio/music/down_under_flopdisk.ogg", 4.0F);
		break;

	default:
		scene_number = 1;
		App->map->Load(map1.GetString());

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
	/*static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}

	*/

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::MintCream)
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		App->entityManager->Destroy_all();
		load_from_save = true;
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
		App->map->Unload();;
		App->fade->FadeToBlack(this, this, 2);
	}

	if ((App->input->GetKey(SDL_SCANCODE_F3)) == KEY_DOWN) {
		
		App->fade->FadeToBlack(2);
		App->render->camera.x = App->render->camera.y = 0;
	}



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
bool j1Scene::PostUpdate()
{
	BROFILER_CATEGORY("Scene Post-Update", Profiler::Color::MediumSlateBlue)
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		ret = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && inGameMenu == false) {
		
		SDL_Rect* rect = new SDL_Rect{ 32, 543, 419, 449 };
		UIElement* principalWindow = App->gui->CreateUIWindow(50, 50, nullptr, rect, true);
		
		uiElements[0] = principalWindow;

		uiElements[1] = App->gui->CreateScrollBar(80, 80, principalWindow, nullptr, /*new SDL_Rect{ 975, 788, 6, 163 }*/new SDL_Rect{ 136, 600, 30, 120 }, new SDL_Rect{ 842, 328, 16, 13 }, new SDL_Rect{ 1003, 437, 16, 13 }, true);

		j1Module* listeners[10];
		memset(listeners, NULL, 10);

		uiElements[2] = App->gui->CreateButton(120, 100, principalWindow, listeners, new SDL_Rect{ 642,169,229,69 }, new SDL_Rect{ 0,113,229,69 }, new SDL_Rect{ 411,169,229,69 }, true);

		inGameMenu = true;
	}

	else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && inGameMenu == true) {

		for (int i = 0; i < 15; i++)
		{
			if (uiElements[i] != nullptr)
			{
				App->gui->DeleteElement(uiElements[i]);
				uiElements[i] = nullptr;
			}
		}
		
		inGameMenu = false;
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
	/*check what UI_element are you getting and do whatevs you want.

	if(UI_element.name == "button to change map"){change scene;}

	etc.
	*/

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