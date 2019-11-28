#include "j1App.h"
#include "AlienEnemy.h"
#include "j1Enemies.h"
#include "j1Collision.h"
#include "j1Map.h"

Alien_Enemy::Alien_Enemy(int x, int y) : Enemy(x, y)
{
	//load the animation

	fly = App->enemy->alienAnimation;

	
	animation = &fly;

	My_ID = 1; //pls fix
	App->enemy->enemies[My_ID - 1];

	collider = App->colliders->AddCollider({ x, y, fly.frames->frame.w, fly.frames->frame.h }, COLLIDER_ENEMY, (j1Module*)App->enemy);

	type = EntityTypes::FLY_ENEMY;
}

void Alien_Enemy::Move(iPoint destiny, float dt)
{
	position.x = App->render->Lerp(position.x, destiny.x, dt);
	position.y = App->render->Lerp(position.y, destiny.y, dt);
}

void Alien_Enemy::LockOn(iPoint destiny, float dt) {
	position.x = App->render->Lerp(position.x, destiny.x, dt);
	position.y = App->render->Lerp(position.y, destiny.y, dt);
}

bool Alien_Enemy::CheckLockOn(iPoint destiny) {
	
	bool ret = false;

	if (position.DistanceTo(destiny) <= 2 * App->map->data.tile_width) { ret = true; }

	return ret;
}

void Alien_Enemy::OnCollision(Collider* collider) {

	if (collider->type == COLLIDER_EXPLOSION)
	{

		Destroy();

	}

}

bool Alien_Enemy::Start() {

	texture = App->enemy->spritesFlyAlien;

	return true;
}