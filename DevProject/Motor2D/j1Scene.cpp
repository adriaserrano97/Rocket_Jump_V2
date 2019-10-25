#include "p2Defs.h"
#include "p2Log.h"
#include "p2List.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1FadeToBlack.h"
#include "j1Scene.h"
#include "j1Player.h"

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
	//Our different maps. We only load the one we're currently using.
	switch (scene_number) {

	case 1:
		App->map->Load("first_map_v2.tmx");

		App->audio->PlayMusic("audio/music/ace_of_flopdisks.wav", 4.0F);
		break;

	case 2:
		App->map->Load("second_map.tmx");
		App->audio->PlayMusic("audio/music/down_under_flopdisk.wav", 4.0F);
		break;

	default:
		scene_number = 1;
		App->map->Load("first_map_v2.tmx"); 
		App->audio->PlayMusic("audio/music/ace_of_flopdisks.wav", 4.0F);
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
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
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

	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		App->map->data.width, App->map->data.height,
		App->map->data.tile_width, App->map->data.tile_height,
		App->map->data.tilesets.count());

	App->win->SetTitle(title.GetString());

	//Change scene logic
	if ((App->input->GetKey(SDL_SCANCODE_F1)) == KEY_DOWN) {
		scene_number = 1;
		
		
		App->map->Unload();
		App->fade->FadeToBlack(this, this, 2);
		App->player->position = App->player->startPos;
	}

	if ((App->input->GetKey(SDL_SCANCODE_F2)) == KEY_DOWN) {
		scene_number = 2;
		
		
		App->map->Unload();;
		App->fade->FadeToBlack(this, this, 2);
		App->player->position = App->player->startPos;
	}

	if ((App->input->GetKey(SDL_SCANCODE_F3)) == KEY_DOWN) {
		
		App->fade->FadeToBlack(2);
		App->player->position = App->player->startPos;
		App->render->camera.x = App->render->camera.y = 0;
	}


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

void j1Scene::OnCollision(Collider* c1, Collider* c2) {

	if (c1->type == COLLIDER_END_LEVEL && c2->type == COLLIDER_PLAYER)
	{
		if (scene_number == 1)
		{
			scene_number = 2;
			
			App->map->Unload();
			App->player->position = App->player->startPos;
			App->fade->FadeToBlack(this, this, 2);
			
		}

		else if (scene_number == 2)
		{
			scene_number = 1;
		
			App->map->Unload();
			App->player->position = App->player->startPos;
			App->fade->FadeToBlack(this, this, 2);
			
		}
	}
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