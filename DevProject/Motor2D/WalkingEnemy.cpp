#include "j1App.h"
#include "WalkingEnemy.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1EntityManager.h"
#include "j1Render.h"



Walking_Enemy::Walking_Enemy(int x, int y) 
{
	//load the animation
	position.x = x;
	position.y = y;

	animation = App->entityManager->walkingAlien;;

	collider = App->colliders->AddCollider({ x, y, animation.GetRect().w, animation.GetRect().h }, COLLIDER_ENEMY, (j1Module*)App->entityManager);
																															//not enemies >:(
	speed = 0.03;

	type = EntityTypes::WALK_ENEMY;
}

void Walking_Enemy::Move(iPoint destiny, float dt)
{
	position.x = App->render->Full_Lerp(position.x, destiny.x, speed, dt);
	
}

void Walking_Enemy::OnCollision(Collider* collider) {

	if (collider->type == COLLIDER_EXPLOSION)
	{
		Destroy();
	}

	if (collider->type == COLLIDER_WALL || collider->type == COLLIDER_TRANSPASSABLE_WALL)
	{

		
		switch (App->entityManager->checkDirection(collider->rect, this->collider->rect))
		{
		case DIRECTION_LEFT:
			position.x = collider->rect.x + collider->rect.w +1;
			path = nullptr;
			in_path = false;


			break;

		case DIRECTION_RIGHT:

			position.x = collider->rect.x - this->collider->rect.w -1;
			path = nullptr;
			in_path = false;

		

			break;

		case DIRECTION_DOWN:

			position.y = collider->rect.y - collider->rect.h + 1;

			

			break;

		case DIRECTION_UP:

			position.y = collider->rect.y - collider->rect.h -1;

			

			break;
		}
	}
}

bool Walking_Enemy::Start() {

	texture = App->entityManager->spritesWalkAlien;
	started = true;

	return true;
}

//Pathfinding, specific of walking enemy

void Walking_Enemy::LockOn(iPoint destiny, float dt) {

	position.x = App->render->Full_Lerp(position.x, destiny.x, speed, dt); //nerfed speed

}

void Walking_Enemy::FollowPath(float dt) {

	if (in_path == true && path->Count() != 0) {

		//make them follow their path

		int tilenum = (path->Count() - 1); // -1 because this returns count, we want to access to array position

		iPoint destiny = App->map->PosConverter(path->At(tilenum)->x, path->At(tilenum)->y);

		destiny.y += round(App->map->data.tile_height / 3); // slight adjustment so the enemy chases the player, not the corner

		Move(destiny, dt);

		iPoint last_tile;

		//if they reached a tile on their path, pop it from their current path

		if (abs(position.x-destiny.x) <= App->entityManager->delta_move || abs(position.x - destiny.x) <= App->entityManager->delta_move + collider->rect.w) {

			path->Pop(last_tile);

			//done pathfinding? Try to pathfind again
			if (path->Count() == 0) { in_path = false; }

		}
	}
}

bool Walking_Enemy::CheckLockOn(iPoint destiny) {
	
	bool ret = false;

	if ((abs(position.x - destiny.x) <= App->map->data.tile_width)) { ret = true; } //only if in melee

	return ret;

}

void Walking_Enemy::AvoidStuck(iPoint destiny) {
	
	iPoint aux;
	while (path->Count() != 0) {
		
		path->Pop(aux);
	
	}
	
	in_path = true;

}

void Walking_Enemy::CheckStuck() {

	if (abs(position.x - position_buffer.x) <= 2*App->entityManager->delta_move && in_path == true) {

		frames_stuck++;
	}
	if (frames_stuck > 5) {

		AvoidStuck(App->player->position);

		frames_stuck = 0;
	}
}
