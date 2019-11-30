#pragma once

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"

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
		MAX
	};
	
	iPoint position;
	EntityTypes type;
	SDL_Texture* texture = nullptr;
	bool to_delete = false;


public:
	Entity(EntityTypes type);
	Entity() {};
	//virtual ~Entity();
	virtual bool HandleInput();
	virtual bool Update(float);
	virtual bool PostUpdate();
	virtual bool Start();
	virtual void Draw(float dt);

	virtual void OnCollision(Collider* collider);

	virtual Collider* GetCollider() const;

};


#endif//__ENTITY_H__
