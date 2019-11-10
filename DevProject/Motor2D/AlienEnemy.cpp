#include "j1App.h"
#include "AlienEnemy.h"
#include "j1Enemies.h"
#include "j1Collision.h"


Alien_Enemy::Alien_Enemy(int x, int y) : Enemy(x, y)
{
	//charge the animation

	collider = App->colliders->AddCollider({ x, y, 50, 50 }, COLLIDER_ENEMY, (j1Module*)App->enemy);

	fly = App->enemy->alienAnimation;

	animation = &fly;

}

void Alien_Enemy::Move()
{
	position.x;
	position.y;
}
