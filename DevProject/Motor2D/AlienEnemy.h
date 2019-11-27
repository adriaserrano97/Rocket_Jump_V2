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

	void Move(iPoint destiny, float dt);
	void OnCollision(Collider* collider);
	void LockOnPlayer(float dt); //Zoom in on player if extremely close. Avoid cluncky movement in short distance
	bool Start();
};

#endif // __ALIEN_ENEMY_H__
