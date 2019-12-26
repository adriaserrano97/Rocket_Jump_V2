#include "j1App.h"
#include "WalkingEnemy.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1EntityManager.h"
#include "j1Render.h"



Walking_Enemy::Walking_Enemy(int x, int y) 
{
	//load the animation
	position.x = x;
	position.y = y;


	position_buffer = position;

	to_delete = false;
	started = false;
	in_path = false;
	myflip = false;

	texture = nullptr;
	path = nullptr;

	frames_stuck = 0.0f;
	path_counter = 0.0f;

	animation = App->entityManager->walkingAlien;

	collider = App->colliders->AddCollider({ x, y, animation.GetRect().w, animation.GetRect().h }, COLLIDER_ENEMY, (j1Module*)App->entityManager);
																															//not enemies >:(
	speed = App->entityManager->enemy_speed;

	type = EntityTypes::WALK_ENEMY;
}


bool Walking_Enemy::Start() {

	texture = App->entityManager->spritesWalkAlien;
	started = true;

	return true;
}


void Walking_Enemy::Move(iPoint destiny, float dt)
{
	iPoint pos_aux = position; //pos before moving

	position.x = App->render->Full_Lerp(position.x, destiny.x, speed, dt);
	position.y += round(App->entityManager->grav * dt);

	iPoint pos_prevision = position; //pos after moving

	if (sgn(position.x - destiny.x) == 1) {
		pos_prevision.x -=App->map->data.tile_width/3 ; //next tile you're expected to reach
	}
	else {
      		pos_prevision.x += (App->map->data.tile_width/3 + collider->rect.w); //next tile you're expected to reach
	}

	if ((App->pathfinding->IsWalkable(App->map->WorldToMap(pos_prevision.x, pos_prevision.y))) != true) {
		position.x = pos_aux.x;
		
	}
}

void Walking_Enemy::OnCollision(Collider* collider) {

	if (collider->type == COLLIDER_EXPLOSION)
	{
		Destroy();
	}

	if (collider->type == COLLIDER_PLAYER)
	{
		Destroy();
	}

	if (collider->type == COLLIDER_WALL || collider->type == COLLIDER_TRANSPASSABLE_WALL)
	{

		
		switch (App->entityManager->checkDirection(collider->rect, this->collider->rect))
		{
		case DIRECTION_LEFT:

			position.x = collider->rect.x + collider->rect.w +1;
			
			if (this->myflip == false) {
				path = nullptr;
				in_path = false;
			}
			
			break;

		case DIRECTION_RIGHT:

			position.x = collider->rect.x - this->collider->rect.w -1;
			
			if (this->myflip == true) {
				path = nullptr;
				in_path = false;
			}
			
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

//Pathfinding, specific of walking enemy

void Walking_Enemy::LockOn(iPoint destiny, float dt) {

	position.x = App->render->Full_Lerp(position.x, destiny.x, speed, dt); 

}

void Walking_Enemy::FollowPath(float dt) {

	if (in_path == true && path->Count() != 0) {

		//make them follow their path

		int tilenum = (path->Count() - 1); // -1 because this returns count, we want to access to array position

		iPoint destiny = App->map->PosConverter(path->At(tilenum)->x, path->At(tilenum)->y);

		destiny.y += round(App->map->data.tile_height / 2); // we want enemy to chase the center of the tile
		destiny.x += round(App->map->data.tile_width / 2);

		Move(destiny, dt);

		iPoint last_tile;

		//if they reached a tile on their path, pop it from their current path

		if (abs(position.x-destiny.x) <= App->entityManager->delta_move || abs(position.x - destiny.x) <= App->entityManager->delta_move + collider->rect.w) {

			path->Pop(last_tile);

			//done pathfinding? Try to pathfind again
   			if (path->Count() == 0) {
				in_path = false; }

		}
	}
}

bool Walking_Enemy::CheckLockOn(iPoint destiny) {
	
	bool ret = false;
	iPoint aux(position.x + this->collider->rect.w, position.y);
	//if ((abs(position.x - destiny.x) <= App->map->data.tile_width)) { ret = true; } //only if in melee
	if (position.DistanceTo(App->entityManager->playerPosition) <= App->map->data.tile_width) { ret = true; } //only if in melee
	else if (aux.DistanceTo(App->entityManager->playerPosition) <= App->map->data.tile_width) { ret = true; } //only if in melee

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

		AvoidStuck(App->entityManager->playerPosition);

		frames_stuck = 0;
	}
}
