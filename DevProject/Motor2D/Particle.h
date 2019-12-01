#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "p2Point.h"
#include "Animation.h"
#include "Entity.h"

struct SDL_Texture;

class Particle: public Entity
{
public:

	uint fx = 0u;
	float life = 0.0f;
	bool flip = false;
	uint sfx = 0u;
	

public:
	~Particle();

};




#endif	__PARTICLE_H__