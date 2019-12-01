#pragma once

#ifndef __J1ENTITY_MANAGER_H__
#define __J1ENTITY_MANAGER_H__

#define MAX_ENTITYES 35

#include "j1Module.h"
#include "Entity.h"
#include "p2DynArray.h"
#include "j1Player.h"

class j1EntityManager : public j1Module
{
public:
	j1EntityManager();
	~j1EntityManager();
	void Destroy_all();

	Entity* CreateEntity(Entity::EntityTypes type, int x, int y);
	void DestroyDeletedEntity();
	bool Save(pugi::xml_node& data) const;
	bool Load(pugi::xml_node& data);

	bool Awake(pugi::xml_node&);
	bool Start();

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	void OnCollision(Collider* c1, Collider* c2);
	COLLISION_WALL_DIRECTION checkDirection(SDL_Rect enemy, SDL_Rect collision);

	bool CleanUp();

	//Enemy
	SDL_Texture* spritesFlyAlien = nullptr;
	SDL_Texture* spritesWalkAlien = nullptr;
	int aggro_range;
	float delta_move; //defines when do enemies correct their pathfinding
	float enemy_speed;
	Animation alienAnimation;
	Animation walkingAlien;

	//Particles
	SDL_Texture* spritesDust = nullptr;
	Animation dustAnimation;
	Animation explosionAnimation;
	float explosion_life;
	float dust_life;

	//Player
	
	SDL_Texture* graphics = nullptr;
	SDL_Texture* bazooka = nullptr;
	bool godMode = false;

	SDL_Rect playerColRect;
	SDL_Rect bazookaRect;
	SDL_Rect cursorRect;

	Animation player_walk;
	Animation player_idle;
	Animation player_jump;
	Animation player_dead;


	uint fsx = 0u;

	int speed;
	int jumpspeed;
	int rocketJumpSpeed;
	int speedcap;
	int grav;
	int deadFall;
	int deadTimer;
	int explosion_CD;
	int JumpingDelta;
	float JumpAdjustMargin;

	iPoint playerPosition;

private:
	Entity* entity_array[MAX_ENTITYES];

	p2SString			folder;

	p2SString			entity_doc;
	p2SString			enemy_folder;
	p2SString			particle_folder;
	p2SString			player_folder;

	float deltaTime;

};



#endif//__J1ENTITY_MANAGER_H__