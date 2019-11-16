#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Enemies.h"
#include "j1Particles.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "Enemy.h"


#define SPAWN_MARGIN 50

j1Enemies::j1Enemies()
{
	name.create("enemy");

	for (uint i = 0; i < MAX_ENEMIES; ++i)
		enemies[i] = nullptr;
}

// Destructor
j1Enemies::~j1Enemies()
{

}

bool j1Enemies::Awake(pugi::xml_node& config) {

	folder.create(config.child("folder").child_value());
	// Create a prototype for each enemy available so we can copy them around

	alienAnimation = alienAnimation.PushAnimation(config, "alienFly");
	walkingAlien = walkingAlien.PushAnimation(config, "alienRun");
	aggro_range = config.child("values").attribute("aggro_range").as_int();
	delta_move = config.child("values").attribute("delta_move").as_int(); //defines when do enemies correct their pathfinding

	return true;
}

bool j1Enemies::Start()
{
	spritesFlyAlien = App->tex->Load(PATH(folder.GetString(), "AlienSprites.png"));
	spritesWalkAlien = App->tex->Load(PATH(folder.GetString(), "WalkingEnemySprites.png"));
	
	

	// check camera position to decide what to spawn
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (queue[i].type != ENEMY_TYPES::NO_TYPE)
		{
			SpawnEnemy(queue[i]);
			//queue[i].type = ENEMY_TYPES::NO_TYPE;
			LOG("Spawning enemy at %d", queue[i].x * App->win->scale);
		}
	}

	return true;
}

bool j1Enemies::PreUpdate()
{
	//Here, we used to spawn all enemies every frame. Let it be remembered.

	return true;
}

// Called before render is available
bool j1Enemies::Update(float dt)
{
	//They move only if they have a path, not on update. -Adri
	/*
	for (uint i = 0; i < MAX_ENEMIES; ++i)
		if (enemies[i] != nullptr) enemies[i]->Move();
	*/ 
	for (uint i = 0; i < MAX_ENEMIES; ++i) {

		if (enemies[i] != nullptr && queue[i].type == ALIEN) enemies[i]->Draw(spritesFlyAlien, dt);
		if (enemies[i] != nullptr && queue[i].type == WALKING_ALIEN) enemies[i]->Draw(spritesWalkAlien, dt);

	}
	
	//check if any enemy has detected the player. If so, pathfind to it.
	for (uint i = 0; i < MAX_ENEMIES; ++i) {

		if (enemies[i] != nullptr) {
		
			if (enemies[i]->position.DistanceTo(App->player->position) < aggro_range && queue[i].in_path == false) {
			
				//Prepare our inputs to create Path
				iPoint o = App->map->WorldToMap(enemies[i]->position.x, enemies[i]->position.y);
				iPoint d = App->map->WorldToMap(App->player->position.x, App->player->position.y);
				
				//We dont want our enemies creating one path a frame
				App->pathfinding->CreatePath(o,d);
				queue[i].path = App->pathfinding->GetLastPath();
				//PROBLEM: this has to be const
				queue[i].in_path = true;
			}
		}
	}

	//make enemies follow their path
	
	for (uint i = 0; i < MAX_ENEMIES; ++i) {

		if (enemies[i] != nullptr && queue[i].in_path == true) {
			
			//make them follow their path
			iPoint destiny = App->map->PosConverter(queue[i].path->At(0)->x, queue[i].path->At(0)->y);

			enemies[i]->Move(destiny); 

			iPoint last_tile;

			//if they reached a tile on their path, pop it from their current path

			if (enemies[i]->position.DistanceTo(destiny) <= delta_move) {

				//queue[i].path->Pop(last_tile);
				//PROBLEM: this cant be const
			
			}
		}
	}


	
	//Print the path, just debug
	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->PosConverter(path->At(i)->x, path->At(i)->y);
		App->render->Blit(App->scene->debug_tex, pos.x, pos.y);
	}
	
	
	
	return true;
	
}


// Called before quitting
bool j1Enemies::CleanUp()
{
	LOG("Freeing all enemies");

	App->tex->UnLoad(spritesFlyAlien);
	App->tex->UnLoad(spritesWalkAlien);

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			delete enemies[i];
			enemies[i] = nullptr;
		}
	}

	//delete enemies; TODO: ask marc about possible delete of those arrays

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (queue[i].type != ENEMY_TYPES::NO_TYPE)
		{
			queue[i].type = ENEMY_TYPES::NO_TYPE;
			queue[i].x = NULL;
			queue[i].y = NULL;
		}

	}

	//delete queue;

	alienAnimation = Animation();
	walkingAlien = Animation();

	return true;
}

bool j1Enemies::AddEnemy(ENEMY_TYPES type, int x, int y)
{
	bool ret = false;

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (queue[i].type == ENEMY_TYPES::NO_TYPE)
		{
			queue[i].type = type;
			queue[i].x = x;
			queue[i].y = y;
			ret = true;
			break;
		}
	}

	return ret;
}

void j1Enemies::SpawnEnemy(const EnemyInfo& info)
{
	// find room for the new enemy
	uint i = 0;
	for (; enemies[i] != nullptr && i < MAX_ENEMIES; ++i);

	if (i != MAX_ENEMIES)
	{
		switch (info.type)
		{
		case ENEMY_TYPES::ALIEN:
			enemies[i] = new Alien_Enemy(info.x, info.y);
			break;

		case ENEMY_TYPES::WALKING_ALIEN:
			enemies[i] = new Walking_Enemy(info.x, info.y);
			break;
		}
	}
}

void j1Enemies::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr && enemies[i]->GetCollider() == c1)
		{
			enemies[i]->OnCollision(c2);
			break;
		}
	}
}