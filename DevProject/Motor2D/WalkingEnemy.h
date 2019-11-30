#ifndef __WALKING_ENEMY_H__
#define __WALKING_ENEMY_H__

#include "Enemy.h"

class Walking_Enemy : public Enemy
{
private:
	bool going_up = true;

public:

	Walking_Enemy(int x, int y);

	void Move(iPoint destiny, float dt);
	void OnCollision(Collider* collider);
	bool Start();

	//Pathfinding walking specific
	void LockOn(iPoint destiny, float dt);
	void FollowPath(float dt);
	void AvoidStuck(iPoint destiny);
	void CheckStuck();
	bool CheckLockOn(iPoint destiny);
	//void Jump();
};

#endif // __ALIEN_ENEMY_H__

