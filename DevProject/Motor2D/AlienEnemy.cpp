#include "j1App.h"
#include "AlienEnemy.h"
#include "j1Enemies.h"
#include "j1Collision.h"


Alien_Enemy::Alien_Enemy(int x, int y) : Enemy(x, y)
{
	//charge the animation

	fly = App->enemy->alienAnimation;

	animation = &fly;

	collider = App->colliders->AddCollider({ x, y, fly.frames->frame.w, fly.frames->frame.h }, COLLIDER_ENEMY, (j1Module*)App->enemy);

	
}

void Alien_Enemy::Move()
{
	position.x;
	position.y;
}

void Alien_Enemy::OnCollision(Collider* collider) {



}