#include "j1App.h"
#include "Enemy.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Enemies.h"
#include "j1Player.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Pathfinding.h"

Enemy::Enemy(int x, int y) : position(x, y) : Entity()
{}

Enemy::Enemy(int x, int y, uint ID) : position(x, y), My_ID(ID)
{}

Enemy::Enemy() : position(0, 0)
{}

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

	if (animation != nullptr)
	{
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
	if (frames_stuck > 2) {

		AvoidStuck(App->player->position);	

		frames_stuck = 0;
	}
}

void Enemy::Pathfind() {

	if (position.DistanceTo(App->player->position) < App->enemy->aggro_range && in_path == false) {
		//remember to turn in-path to true every 60 frames or so, so enemies can re-calculate their path

		LOG("the saddest of yeeets");
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

//Rendering functions
void Enemy::Draw(SDL_Texture* sprites, float dt)
{
	if (collider != nullptr) {
		collider->SetPos(position.x, position.y);
	}

	if (animation != nullptr) {
		App->render->Blit(sprites, position.x, position.y, &animation->GetCurrentFrameBox(dt));
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


//collision

COLLISION_WALL_DIRECTION Enemy::checkDirection(SDL_Rect enemy, SDL_Rect collision) {

	int directionDiference[DIRECTION_MAX];

	directionDiference[DIRECTION_LEFT] = abs((enemy.x + enemy.w) - collision.x);
	directionDiference[DIRECTION_RIGHT] = abs((collision.x + collision.w) - enemy.x);
	directionDiference[DIRECTION_UP] = abs((enemy.y + enemy.h) - collision.y);
	directionDiference[DIRECTION_DOWN] = abs((collision.y + collision.h) - enemy.y);

	int directionCheck = DIRECTION_NONE;

	for (int i = 0; i < DIRECTION_MAX; ++i)
	{
		if (directionCheck == DIRECTION_NONE)
			directionCheck = i;
		else if ((directionDiference[i] < directionDiference[directionCheck]))
			directionCheck = i;

	}

	return (COLLISION_WALL_DIRECTION)directionCheck;
}

void Enemy::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (App->enemy->enemies[i] != nullptr && App->enemy->enemies[i]->GetCollider() == c1)
		{
			OnCollision(c2);

			switch (checkDirection(c1->rect, c2->rect))
			{
			case DIRECTION_LEFT:

				position.x = c2->rect.x - c1->rect.w - 1;

				in_path = false;

				break;

			case DIRECTION_RIGHT:

				position.x = c2->rect.x + c2->rect.w + 1;

				in_path = false;

				break;

			case DIRECTION_DOWN:

				position.y = c2->rect.y + c2->rect.h + 1;

				in_path = false;

				break;

			case DIRECTION_UP:

				position.y = c2->rect.y - c1->rect.h - 1;

				in_path = false;

				break;
			}

			break;
		}
	}
}

const Collider* Enemy::GetCollider() const
{
	return collider;
}
