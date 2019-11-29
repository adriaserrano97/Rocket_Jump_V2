#include "j1App.h"
#include "Enemy.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Enemies.h"
#include "j1Player.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Pathfinding.h"


Enemy::~Enemy()
{
	collider = nullptr;

	if (animation != nullptr)
	{
		*animation = Animation();
	}
}

void Enemy::Destroy() {

	if (collider != nullptr) {

		collider->to_delete = true;
	}

	if (animation != nullptr) {

		*animation = Animation(); 
	}
}

//Pathfinding 

void Enemy::AvoidStuck(iPoint destiny) {

	position.x += 10 * sgn(destiny.x - position.x);

    position.y += 10 * sgn(destiny.y - position.y);
}

void Enemy::CheckStuck() {

	if (position.DistanceTo(position_buffer) < App->enemy->delta_move && in_path == true) {

		frames_stuck++;
	}
	if (frames_stuck > 30) {

		AvoidStuck(App->player->position);	

		frames_stuck = 0;
	}
}

void Enemy::Pathfind(float dt) {

	if (CheckLockOn(App->player->position)) {
		
		iPoint aux(App->player->collider->rect.x + App->player->collider->rect.w/2, App->player->collider->rect.y + App->player->collider->rect.h/2);
		
		LockOn(aux, dt);

	}else if (position.DistanceTo(App->player->position) < App->enemy->aggro_range && in_path == false) {
		//remember to turn in-path to true every 60 frames or so, so enemies can re-calculate their path

		//Prepare our inputs to create Path
		iPoint o = App->map->WorldToMap(position.x, position.y);
		iPoint d = App->map->WorldToMap(App->player->position.x, App->player->position.y);

		//We dont want our enemies creating one path a frame
		App->pathfinding->CreatePath(o, d);
		path = &App->pathfinding->last_path;
		path->Flip(); //this ensures that Pop() pops starting from the enemy, thus enabling pathfinding
		in_path = true;
	}
}

void Enemy::FollowPath(float dt) {

	if (in_path == true && path->Count() != 0) {

		//make them follow their path

		int tilenum = (path->Count() - 1); // -1 because this returns count, we want to access to array position
		iPoint destiny = App->map->PosConverter(path->At(tilenum)->x,path->At(tilenum)->y);
		destiny.y += App->map->data.tile_height / 3; // slight adjustment so the enemy chases the player, not the corner
		Move(destiny, dt);

		iPoint last_tile;

		//if they reached a tile on their path, pop it from their current path

		if (position.DistanceTo(destiny) <= App->enemy->delta_move || position.DistanceTo(destiny) <= App->enemy->delta_move + collider->rect.w) {

			path->Pop(last_tile);

			//done pathfinding? Try to pathfind again
			if (path->Count() == 0) { in_path = false; }

		}
	}
}

void Enemy::ResetPathCounter(float dt) {

	path_counter += 1 + dt;

	if (path_counter > 300) {
	
		path_counter = 0;

		path = nullptr;

		in_path = false;
	}
}

bool Enemy::CheckLockOn(iPoint destiny) {

	//each enemy has its own conditions

	return false;

}

//Rendering functions
void Enemy::Draw(float dt)
{
	if (collider != nullptr) {
		collider->SetPos(position.x, position.y);
	}

	if (animation != nullptr) {
		App->render->Blit(texture, position.x, position.y, &animation->GetCurrentFrameBox(dt));
	}
}

//Enemy managment between games
bool Enemy::Save(pugi::xml_node& data) const {


	//TODO: REWRITE THIS ONCE WE HAVE ENTITY MANAGER

	data.append_attribute("x") = position.x;
	data.append_attribute("y") = position.y;
	//remember type too
	

	return true;
	
}





Collider* Enemy::GetCollider() const
{
	return collider;
}


bool Enemy::HandleInput() {

	CheckStuck();
	position_buffer = position; 

	return true;
}


bool Enemy::Update(float dt) {

	Pathfind(dt);
	FollowPath(dt);
	App->pathfinding->PrintLastPath();

	ResetPathCounter(dt); //just reset all paths each X frames. It looks way smoother

	if (type != EntityTypes::FLY_ENEMY) {
		position.y += round(App->player->grav * dt);
	}
	return true;
}


