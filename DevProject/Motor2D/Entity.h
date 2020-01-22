#pragma once

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"
#include "Animation.h"

struct Collider;
struct SDL_Texture;

enum COLLISION_WALL_DIRECTION
{
	DIRECTION_NONE = -1,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_MAX
};

class Entity
{

public:
	
	enum class EntityTypes
	{
		FAIL = -1,
		FLY_ENEMY,
		WALK_ENEMY,
		EXPLOSION_PARTICLE,
		DUST_PARTICLE,
		PLAYER,
		COIN,
		MAX
	};
	
	iPoint position;
	EntityTypes type;
	SDL_Texture* texture;
	bool to_delete;
	Animation animation;
	bool started;


public:
	Entity();
	Entity(iPoint position, EntityTypes type, Animation animation);
	virtual ~Entity();
	virtual bool HandleInput();
	virtual bool Update(float);
	virtual bool Start();
	virtual void Draw(float dt);

	virtual void OnCollision(Collider* collider);

	virtual Collider* GetCollider() const;

};


#endif//__ENTITY_H__
