#include "j1App.h"
#include "WalkingEnemy.h"
#include "j1Enemies.h"
#include "j1Collision.h"


Walking_Enemy::Walking_Enemy(int x, int y) : Enemy(x, y)
{
	//charge the animation

	run = App->enemy->walkingAlien;

	animation = &run;

	collider = App->colliders->AddCollider({ x, y, run.frames->frame.w, run.frames->frame.h }, COLLIDER_ENEMY, (j1Module*)App->enemy);


}

void Walking_Enemy::Move()
{
	position.x;
	position.y;
}


void Walking_Enemy::OnCollision(Collider* collider) {

	if (collider->type == COLLIDER_EXPLOSION)
	{

		Destroy();

	}

}