#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Enemies.h"
#include "j1Particles.h"
#include "j1Textures.h"
#include "j1Window.h"
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

// Called before render is available
bool j1Enemies::Update(float dt)
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
		if (enemies[i] != nullptr) enemies[i]->Move();

	for (uint i = 0; i < MAX_ENEMIES; ++i) {

		if (enemies[i] != nullptr && queue[i].type == ALIEN) enemies[i]->Draw(spritesFlyAlien);
		if (enemies[i] != nullptr && queue[i].type == WALKING_ALIEN) enemies[i]->Draw(spritesWalkAlien);

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