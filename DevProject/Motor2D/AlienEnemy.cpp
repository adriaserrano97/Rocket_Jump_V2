#include "j1App.h"
#include "AlienEnemy.h"
#include "j1EntityManager.h"
#include "j1Collision.h"
#include "j1Map.h"
#include"j1EntityManager.h"
#include "j1Render.h"

Alien_Enemy::Alien_Enemy(int x, int y) //: Enemy(x, y)
{
	//load the animation

	position.x = x;
	position.y = y;
	
	animation = App->entityManager->alienAnimation;

	collider = App->colliders->AddCollider({ x, y, animation.GetRect().w, animation.GetRect().h }, COLLIDER_ENEMY, (j1Module*)App->entityManager);
	
	type = EntityTypes::FLY_ENEMY;
}

void Alien_Enemy::Move(iPoint destiny, float dt)
{
	position.x = App->render->Lerp(position.x, destiny.x, dt);
	position.y = App->render->Lerp(position.y, destiny.y, dt);
}

void Alien_Enemy::OnCollision(Collider* collider) {

	if (collider->type == COLLIDER_EXPLOSION)
	{

		Destroy();

	}
	if (collider->type == COLLIDER_WALL || collider->type == COLLIDER_TRANSPASSABLE_WALL)
	{
		//OnCollision(c2);

		switch (App->entityManager->checkDirection(collider->rect, this->collider->rect))
		{
		case DIRECTION_LEFT:
			position.x = collider->rect.x + collider->rect.w + 1;
			
			path = nullptr;
			in_path = true;
			
			break;

		case DIRECTION_RIGHT:

			position.x = collider->rect.x - this->collider->rect.w - 1;

			path = nullptr;
			in_path = true;
			
			break;

		case DIRECTION_DOWN:

			position.y = collider->rect.y - collider->rect.h + 1;



			break;

		case DIRECTION_UP:

			position.y = collider->rect.y - this->collider->rect.h - 1;



			break;
		}
	}
}

bool Alien_Enemy::Start() {

	texture = App->entityManager->spritesFlyAlien;
	started = true;

	return true;
}

void Alien_Enemy::LockOn(iPoint destiny, float dt) {

	position.x = App->render->Lerp(position.x, destiny.x, dt);
	position.y = App->render->Lerp(position.y, destiny.y, dt);
}

bool Alien_Enemy::CheckLockOn(iPoint destiny) {
	
	bool ret = false;

	if (position.DistanceTo(destiny) <= 2*App->map->data.tile_width) { 
		ret = true; }

	return ret;
}
