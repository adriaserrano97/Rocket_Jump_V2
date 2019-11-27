#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Enemies.h"
#include "j1Particles.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Pathfinding.h"
#include "p2Log.h"


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
	delta_move = config.child("values").attribute("delta_move").as_float();
	

	return true;
}

bool j1Enemies::Start()
{
	spritesFlyAlien = App->tex->Load(PATH(folder.GetString(), "AlienSprites.png"));
	spritesWalkAlien = App->tex->Load(PATH(folder.GetString(), "WalkingEnemySprites.png"));
	return true;
}

bool j1Enemies::PreUpdate()
{
	//Here, we used to spawn all enemies every frame. Let it be remembered.

	for (uint i = 0; i < MAX_ENEMIES; ++i) {

		if (enemies[i] != nullptr) {

			enemies[i]->CheckStuck();
			enemies[i]->position_buffer = enemies[i]->position;
		}
	}

	return true;
}

// Called before render is available
bool j1Enemies::Update(float dt)
{	
	//check if any enemy has detected the player. If so, pathfind to it.
	for (uint i = 0; i < MAX_ENEMIES; ++i) {

		if (enemies[i] != nullptr) {
		
			enemies[i]->Pathfind(dt);
			enemies[i]->FollowPath(dt);
			App->pathfinding->PrintLastPath(); //optional, comment this before release
			

		}
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


	alienAnimation = Animation();
	walkingAlien = Animation();

	return true;
}

//Enemy managment
/*void j1Enemies::SpawnEnemy(ENEMY_TYPES type, int x, int y)
{
	// find room for the new enemy
	uint i = 0;
	for (; App->enemy->enemies[i] != nullptr && i < MAX_ENEMIES; ++i);

	/*if (i != MAX_ENEMIES)
	{
		switch (type)
		{
		case ENEMY_TYPES::ALIEN:
		{
			Alien_Enemy *this_enemy = new Alien_Enemy(x, y);
			this_enemy->My_ID = i;
			this_enemy->type = type;
			App->enemy->enemies[i] = this_enemy;
			break;
		}

		case ENEMY_TYPES::WALKING_ALIEN:
		{
			Walking_Enemy *this_enemy = new Walking_Enemy(x, y);
			this_enemy->My_ID = i;
			this_enemy->type = type;
			App->enemy->enemies[i] = this_enemy;
			break;
		}
		}
	}
}*/


