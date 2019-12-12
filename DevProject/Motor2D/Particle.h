#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "p2Point.h"
#include "Animation.h"
#include "Entity.h"

struct SDL_Texture;

class Particle: public Entity
{
protected:
	float life;
	
public:
	
	void Draw(float dt);
};




#endif	__PARTICLE_H__