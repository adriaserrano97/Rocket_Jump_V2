#ifndef __EXPLOSION_H__
#define __EXPLOSION_H__

#include "Particle.h"

struct Collider;

class Explosion : public Particle
{
private:
	float wave = -1.0f;
	bool going_up = true;
	Collider* collider = nullptr;
public:

	Explosion(int x, int y);
	bool Update(float dt) { return true; }
	void Draw(float dt) {  }

	bool Start() { return true; }
};

#endif // __EXPLOSION_H__

