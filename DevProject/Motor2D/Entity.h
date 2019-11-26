#pragma once

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"

class SDL_Texture;



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
	virtual ~Entity();
	virtual void PreUpdate();
	virtual void Update(float);
	virtual void PostUpdate();

};


#endif//__ENTITY_H__
