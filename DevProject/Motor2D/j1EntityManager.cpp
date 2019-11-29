#include "j1EntityManager.h"
#include "AlienEnemy.h"
#include "WalkingEnemy.h"
#include "p2Log.h"


j1EntityManager::j1EntityManager() : j1Module() {

	name.create("Entity_manager");

	for (int i = 0; i <= MAX_ENTITYES; i++)
	{
		entity_array[i] = nullptr;
	}
}


j1EntityManager::~j1EntityManager() {}


bool j1EntityManager::Awake(pugi::xml_node& config) {
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

bool j1EntityManager::Start() {

	for (int i = 0; i < MAX_ENTITYES && entity_array[i] != nullptr; i++)
	{
		entity_array[i]->Start();
	}

	return true;
}


Entity* j1EntityManager::CreateEntity(Entity::EntityTypes type, int x, int y) {

	Entity* ret = nullptr;
	
	switch (type)
	{
	case Entity::EntityTypes::FLY_ENEMY:	ret = new Alien_Enemy(x, y);	break;
	case Entity::EntityTypes::WALK_ENEMY:	ret = new Walking_Enemy(x, y);	break;
	}


	for (int i = 0; i < MAX_ENTITYES; i++)
	{
		if (entity_array[i] == nullptr)
		{
			entity_array[i] = ret;
			break;
		}

	}
	

	return ret;
}


void j1EntityManager::DestroyEntity(Entity* entity) {

	

}


bool j1EntityManager::PreUpdate() {

	for (int i = 0; i < MAX_ENTITYES && entity_array[i] != nullptr; i++) {

		entity_array[i]->HandleInput();
	}

	return true;
}


bool j1EntityManager::Update(float dt) {

	for (int i = 0; i < MAX_ENTITYES && entity_array[i] != nullptr; i++) {

		entity_array[i]->Update(dt);
	}

	deltaTime = dt;

	return true;
}


bool j1EntityManager::PostUpdate() {

	for (int i = 0; i < MAX_ENTITYES && entity_array[i] != nullptr; i++) {

		entity_array[i]->Draw(deltaTime);
	}

	return true;
}

bool j1EntityManager::CleanUp() {

	for (int i = 0; i < MAX_ENTITYES && entity_array[i] != nullptr; i++) {

		delete entity_array[i];
		entity_array[i] = nullptr;
	}


	return true;
}


//collision

COLLISION_WALL_DIRECTION j1EntityManager::checkDirection(SDL_Rect enemy, SDL_Rect collision) {

	int directionDiference[DIRECTION_MAX];

	directionDiference[DIRECTION_LEFT] = abs((enemy.x + enemy.w) - collision.x);
	directionDiference[DIRECTION_RIGHT] = abs((collision.x + collision.w) - enemy.x);
	directionDiference[DIRECTION_UP] = abs((enemy.y + enemy.h) - collision.y);
	directionDiference[DIRECTION_DOWN] = abs((collision.y + collision.h) - enemy.y);

	int directionCheck = DIRECTION_NONE;

	for (int i = 0; i < DIRECTION_MAX; ++i)
	{
		if (directionCheck == DIRECTION_NONE)
			directionCheck = i;
		else if ((directionDiference[i] < directionDiference[directionCheck]))
			directionCheck = i;

	}

	return (COLLISION_WALL_DIRECTION)directionCheck;
}


void j1EntityManager::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		Collider* col = entity_array[i]->GetCollider();
		if (entity_array[i] != nullptr &&  col== c1)
		{
			entity_array[i]->OnCollision(c2);
			break;
		}
	}
}