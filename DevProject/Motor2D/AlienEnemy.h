#ifndef __ALIEN_ENEMY_H__
#define __ALIEN_ENEMY_H__

#include "Enemy.h"

class Alien_Enemy : public Enemy
{

public:
	Alien_Enemy(int x, int y);
	bool Start();

private:
	void Move(iPoint destiny, float dt);
	void LockOn(iPoint destiny, float dt);
	void OnCollision(Collider* collider);
	bool CheckLockOn(iPoint destiny);
	
};

#endif // __ALIEN_ENEMY_H__
