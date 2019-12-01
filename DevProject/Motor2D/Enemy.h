#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "p2Point.h"

#include "p2DynArray.h"
#include "Entity.h"

struct SDL_Texture;
struct Collider;

class Enemy: public Entity
{
public:
	//general info
	iPoint position_buffer; //position last frame
	
	Collider* collider = nullptr;
	uint My_ID; //position in enemy array

	//pathfinding info
	bool in_path = false;
	float frames_stuck = 0;
	p2DynArray<iPoint>* path = nullptr;

public:
	

	~Enemy();
	void Destroy();


	//Pathfinding functions
	virtual void Move(iPoint pos, float dt) {};
	virtual void LockOn(iPoint destiny, float dt) {};
	virtual bool CheckLockOn(iPoint destiny);
	virtual void AvoidStuck(iPoint destiny);
	virtual void CheckStuck();
	virtual void Pathfind(float dt);
	virtual void FollowPath(float dt);
	virtual void ResetPathCounter(float dt);
	float path_counter = 0;

	bool Update(float dt);
	bool HandleInput();

	//Render fucntion
	void Draw(float dt);
	bool myflip = false;


	//Collision managment
	
	Collider* GetCollider() const;
	
};

#endif // __ENEMY_H__
