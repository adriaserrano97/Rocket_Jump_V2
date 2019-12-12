#ifndef __WALKING_ENEMY_H__
#define __WALKING_ENEMY_H__

#include "Enemy.h"

class Walking_Enemy : public Enemy
{
public:

	Walking_Enemy(int x, int y);
	bool Start();
	
private:
	void Move(iPoint destiny, float dt);

	//Pathfinding walking specific
	void LockOn(iPoint destiny, float dt);
	void FollowPath(float dt);
	void AvoidStuck(iPoint destiny);
	void CheckStuck();
	bool CheckLockOn(iPoint destiny);
	//void Jump();

	void OnCollision(Collider* collider);
};

#endif // __ALIEN_ENEMY_H__

