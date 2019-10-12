#include "p2Defs.h"
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

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	switch (App->fade->scene_number) {
	case 1:
		App->map->Load("first_map_v2.tmx");
		break;
	case 2:
		//img = App->tex->Load("textures/SDL_pls.png"); //used for debug purposes
		App->map->Load("test_small_tileset.tmx");
		break;
	default:
		App->fade->scene_number = 1;
		App->map->Load("first_map_v2.tmx"); //ADRI: we default to map 1. Once we have UI, we will default to the Main Menu
		break;
	}
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->LoadGame();

	if(App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->SaveGame();

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 10;

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 10;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 10;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 10;

	App->map->Draw();

	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count());

	App->win->SetTitle(title.GetString());

	//Change scene logic
	if ((App->input->GetKey(SDL_SCANCODE_1) && (App->fade->scene_number != 1)) == KEY_DOWN) {
		App->fade->scene_number = 1;
		App->map->CleanUp();
		App->fade->FadeToBlack(this, this, 2);
		App->map->Start();
		//App->fade->FadeToBlack((j1Module*)App->map, (j1Module*)App->map,2);
		//ADRI:We can't do this because we need to change information in Map between its cleanup and its new awake
	}
	if ((App->input->GetKey(SDL_SCANCODE_2) && (App->fade->scene_number != 2)) == KEY_DOWN) {
		App->fade->scene_number = 2;
		App->map->CleanUp();
		App->fade->FadeToBlack(this, this, 2);
		App->map->Start();
	}

	//ADRI: We have memory leaks here
	//ADRI: We are deleting all colliders, we need to re-make the player collider

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
