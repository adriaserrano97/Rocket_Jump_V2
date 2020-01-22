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
	Particle(int x, int y, float life, EntityTypes type, Animation animation);
	~Particle();
	void Draw(float dt);
};




#endif	__PARTICLE_H__