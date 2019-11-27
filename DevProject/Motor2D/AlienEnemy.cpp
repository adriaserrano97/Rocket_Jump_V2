#include "j1App.h"
#include "AlienEnemy.h"
#include "j1Enemies.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "j1Map.h"


Alien_Enemy::Alien_Enemy(int x, int y) : Enemy(x, y)
{
	//load the animation

	fly = App->enemy->alienAnimation;

	
	animation = &fly;

	My_ID = 1;

	collider = App->colliders->AddCollider({ x, y, fly.frames->frame.w, fly.frames->frame.h }, COLLIDER_ENEMY, (j1Module*)App->enemy);

	App->enemy->enemies[My_ID-1];

	type = EntityTypes::FLY_ENEMY;
}

void Alien_Enemy::Move(iPoint destiny, float dt)
{
	if (position.DistanceTo(destiny) <= App->map->data.tile_width/2) {
		LockOnPlayer(dt);
	}
	else {

		position.x = App->render->Lerp(position.x, destiny.x, dt);
		position.y = App->render->Lerp(position.y, destiny.y, dt);
	}
}

void Alien_Enemy::OnCollision(Collider* collider) {

	

}

bool Alien_Enemy::Start() {

	texture = App->enemy->spritesFlyAlien;

	return true;
}

void Alien_Enemy::LockOnPlayer(float dt) {

	iPoint aux(App->player->collider->rect.x + App->player->collider->rect.w, App->player->collider->rect.y + App->player->collider->rect.h);

	Move(aux, dt);

}