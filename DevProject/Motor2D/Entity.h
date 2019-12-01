#pragma once

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"
#include "Animation.h"

struct Collider;
struct SDL_Texture;

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
		MAX
	};
	
	iPoint position;
	EntityTypes type;
	SDL_Texture* texture = nullptr;
	bool to_delete = false;
	Animation animation;
	bool started = false;


public:
	Entity(EntityTypes type);
	Entity() {};
	//virtual ~Entity();
	virtual bool HandleInput();
	virtual bool Update(float);
	virtual bool Start();
	virtual void Draw(float dt);

	virtual void OnCollision(Collider* collider);

	virtual Collider* GetCollider() const;

};


#endif//__ENTITY_H__
