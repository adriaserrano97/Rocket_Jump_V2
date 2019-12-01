#ifndef __EXPLOSION_H__
#define __EXPLOSION_H__

#include "Particle.h"

struct Collider;

class Explosion : public Particle
{
private:

	Collider* collider = nullptr;
public:

	~Explosion();
	Explosion(int x, int y);
	Collider* GetCollider() const;

	bool Start();
	bool Update(float dt);
};

#endif // __EXPLOSION_H__

