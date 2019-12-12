#ifndef __DUST_H__
#define __DUST_H__

#include "Particle.h"

class Dust : public Particle
{
public:

	Dust(int x, int y);
	~Dust();

	bool Start();
	bool Update(float dt);
};

#endif // __DUST_H__
