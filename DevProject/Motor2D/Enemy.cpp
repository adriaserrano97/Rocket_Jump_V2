#include "j1App.h"
#include "Enemy.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1EntityManager.h"


Enemy::~Enemy()
{
	collider = nullptr;

	if (path != nullptr)
	{
		path->Clear();
		path = nullptr;
	}
	
	texture = nullptr;

	animation = Animation();
}

void Enemy::Destroy() {

	to_delete = true;

	if (collider != nullptr) {

		collider->to_delete = true;
		collider = nullptr;
	}


	if (path!= nullptr)
	{
		path->Clear();
	}

	animation = Animation();
	
}

void Enemy::Move(iPoint pos, float dt) {}

void Enemy::LockOn(iPoint destiny, float dt) {}

//Pathfinding 

void Enemy::AvoidStuck(iPoint destiny) {

	position.x += 10 * sgn(destiny.x - position.x);

    position.y += 10 * sgn(destiny.y - position.y);
}

void Enemy::CheckStuck() {

	if (position.DistanceTo(position_buffer) < App->entityManager->delta_move && in_path == true) {

		frames_stuck++;
	}
	if (frames_stuck > 30) {

		AvoidStuck(App->entityManager->playerPosition);	

		frames_stuck = 0;
	}
}

void Enemy::Pathfind(float dt) {

	if (CheckLockOn(App->entityManager->playerPosition)) {
		
		iPoint aux(App->entityManager->playerPosition.x + App->entityManager->playerColRect.w/2, App->entityManager->playerPosition.y - App->entityManager->playerColRect.h/2);
		LockOn(aux, dt);

	}else if (position.DistanceTo(App->entityManager->playerPosition) < App->entityManager->aggro_range && in_path == false) {
		//remember to turn in-path to true every 60 frames or so, so enemies can re-calculate their path

		//Prepare our inputs to create Path
		iPoint o = App->map->WorldToMap(position.x, position.y);
		iPoint d = App->map->WorldToMap(App->entityManager->playerPosition.x, App->entityManager->playerPosition.y);

		//We dont want our enemies creating one path a frame
		App->pathfinding->CreatePath(o, d);
		path = &App->pathfinding->last_path;
		path->Flip(); //this ensures that Pop() pops starting from the enemy, thus enabling pathfinding
		in_path = true;
	}
}

void Enemy::FollowPath(float dt) {

	if (in_path == true && path != nullptr) {

		//make them follow their path
		if (path->Count() != 0)
		{

			int tilenum = (path->Count() - 1); // -1 because this returns count, we want to access to array position
			iPoint destiny = App->map->PosConverter(path->At(tilenum)->x, path->At(tilenum)->y);
			destiny.y -= 1; // slight adjustment so the enemy chases the player, not the corner
			Move(destiny, dt);

			iPoint last_tile;

			//if they reached a tile on their path, pop it from their current path

			if (position.DistanceTo(destiny) <= round(App->entityManager->delta_move / 3) || position.DistanceTo(destiny) <= round(App->entityManager->delta_move / 3) + collider->rect.w) {

				path->Pop(last_tile);

				//done pathfinding? Try to pathfind again
				if (path->Count() == 0) {
					in_path = false;
				}

			}
		}
	}
}

void Enemy::ResetPathCounter(float dt) {

	path_counter += 1 + dt;

	if (path_counter > 100) {
	
		path_counter = 0;

		path = nullptr;

		in_path = false;

		frames_stuck = 0;
	}
}

bool Enemy::CheckLockOn(iPoint destiny) {

	//each enemy has its own conditions

	return true;

}

//Rendering functions
void Enemy::Draw(float dt)
{
	if (collider != nullptr) {
		collider->SetPos(position.x, position.y);
	}


		
		App->render->Blit(texture, position.x, position.y, &animation.GetCurrentFrameBox(dt), NULL, NULL, NULL, NULL, myflip);
		
	
	
}


Collider* Enemy::GetCollider() const
{
	return collider;
}


bool Enemy::HandleInput() {

	CheckStuck();
	position_buffer = position;
	if (position.x - App->entityManager->playerPosition.x >= 0) {
		myflip = false;
	}
	else { myflip = true; }
	return true;
}


bool Enemy::Update(float dt) {

	Pathfind(dt);
	FollowPath(dt);
	//App->pathfinding->PrintLastPath();

	ResetPathCounter(dt); //just reset all paths each X frames. It looks way smoother

	if (type != EntityTypes::FLY_ENEMY) {
		position.y += round(App->entityManager->grav * dt);
	}
	return true;
}


