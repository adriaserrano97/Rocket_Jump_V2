#include "j1App.h"
#include "AlienEnemy.h"
#include "j1Enemies.h"
#include "j1Collision.h"


Alien_Enemy::Alien_Enemy(int x, int y) : Enemy(x, y)
{
	//load the animation

	fly = App->enemy->alienAnimation;

	animation = &fly;

	collider = App->colliders->AddCollider({ x, y, fly.frames->frame.w, fly.frames->frame.h }, COLLIDER_ENEMY, (j1Module*)App->enemy);

	int z;
}

void Alien_Enemy::Move(iPoint destiny, float dt)
{
	position.x = App->render->Lerp(position.x, destiny.x, dt);
	position.y = App->render->Lerp(position.y, destiny.y, dt);

}

void Alien_Enemy::OnCollision(Collider* collider) {

	

}