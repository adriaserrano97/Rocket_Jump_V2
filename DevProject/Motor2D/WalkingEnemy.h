#ifndef __WALKING_ENEMY_H__
#define __WALKING_ENEMY_H__

#include "Enemy.h"

class Walking_Enemy : public Enemy
{
private:
	bool going_up = true;
	Animation run;

public:

	Walking_Enemy(int x, int y);

	void Move();
};

#endif // __ALIEN_ENEMY_H__

