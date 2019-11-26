#ifndef __ENEMY_H__
#define __ENEMY_H__

#define MAX_ENEMIES 100

#include "p2Point.h"
#include "Animation.h"
#include "p2DynArray.h"
#include "Entity.h"

struct SDL_Texture;
struct Collider;
enum COLLISION_WALL_DIRECTION;

class Enemy: public Entity
{
public:
	//general info
	iPoint position;
	iPoint position_buffer; //position at start of step
	Animation* animation = nullptr;
	Collider* collider = nullptr;
	uint My_ID; //position in enemy array

	//pathfinding info
	bool in_path = false;
	float frames_stuck = 0;
	p2DynArray<iPoint>* path = nullptr;

public:
	Enemy(int x, int y, uint ID);
	Enemy(int x, int y);
	Enemy();

	virtual ~Enemy();
	void Destroy();


	//Pathfinding functions
	virtual void Move(iPoint pos, float dt) {};
	virtual void AvoidStuck(iPoint destiny);
	virtual void CheckStuck();
	virtual void Pathfind();
	virtual void FollowPath(float dt);

	bool Update(float dt);
	bool HandleInput();
	//Render fucntion
	void Draw(float dt);

	//Enemy managment between games
	bool Save(pugi::xml_node&) const;
	/* ----------------Load function. yikes----------------------*/

	//Collision managment
	void OnCollision(Collider* c1, Collider* c2);
	virtual void OnCollision(Collider* collider) {};
	const Collider* GetCollider() const;
	COLLISION_WALL_DIRECTION checkDirection(SDL_Rect enemy, SDL_Rect collision);
};

#endif // __ENEMY_H__
