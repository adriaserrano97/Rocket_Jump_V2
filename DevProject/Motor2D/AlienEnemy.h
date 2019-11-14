#ifndef __ALIEN_ENEMY_H__
#define __ALIEN_ENEMY_H__

#include "Enemy.h"

class Alien_Enemy : public Enemy
{
private:
	float wave = -1.0f;
	bool going_up = true;
	Animation fly;

public:

	Alien_Enemy(int x, int y);

	void Move();
	void OnCollision(Collider* collider);
};

#endif // __ALIEN_ENEMY_H__