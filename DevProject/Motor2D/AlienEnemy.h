#ifndef __ALIEN_ENEMY_H__
#define __ALIEN_ENEMY_H__

#include "Enemy.h"

class Alien_Enemy : public Enemy
{
private:
	float wave = -1.0f;
	bool going_up = true;

public:

	Alien_Enemy(int x, int y);

	void Move(iPoint destiny, float dt);
	void LockOn(iPoint destiny, float dt);
	void OnCollision(Collider* collider);
	bool CheckLockOn(iPoint destiny);
	bool Start();
};

#endif // __ALIEN_ENEMY_H__
