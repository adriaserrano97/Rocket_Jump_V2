#pragma once

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"

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
	SDL_Texture* texture;


public:
	Entity(EntityTypes type);
	Entity() {};
	//virtual ~Entity();
	virtual bool PreUpdate();
	virtual bool Update(float);
	virtual bool PostUpdate();
	virtual bool Start();
	virtual void Draw(float dt);

};


#endif//__ENTITY_H__
