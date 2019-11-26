#include "j1EntityManager.h"
#include "AlienEnemy.h"
#include "WalkingEnemy.h"
#include "p2Log.h"


j1EntityManager::j1EntityManager() : j1Module() {

	name.create("Entity_manager");
}


j1EntityManager::~j1EntityManager() {}


bool j1EntityManager::Awake(pugi::xml_node& config) {
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

bool j1EntityManager::Start() {

	return true;
}


Entity* j1EntityManager::CreateEntity(Entity::EntityTypes type, int x, int y) {

	Entity* ret = nullptr;
	
	switch (type)
	{
	case Entity::EntityTypes::FLY_ENEMY:	ret = new Alien_Enemy(x, y);	break;
	case Entity::EntityTypes::WALK_ENEMY:	ret = new Walking_Enemy(x, y); break;
	}

	return ret;
}


void j1EntityManager::DestroyEntity(Entity* entity) {


}


bool j1EntityManager::PreUpdate() {

	return true;
}


bool j1EntityManager::Update(float dt) {

	return true;
}


bool j1EntityManager::PostUpdate() {

	return true;
}

bool j1EntityManager::CleanUp() {

	return true;
}