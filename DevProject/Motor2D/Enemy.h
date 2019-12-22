#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "p2Point.h"

#include "p2DynArray.h"
#include "Entity.h"

#define PATH_COUNTER_MAX 100

struct SDL_Texture;
struct Collider;

class Enemy: public Entity
{
public:
	//general info
	iPoint position_buffer; //position last frame
	
	Collider* collider;

	//pathfinding info
protected:
	bool in_path;
	float frames_stuck;
	float path_counter;
	p2DynArray<iPoint>* path;
	bool myflip;
	float speed;

public:
	~Enemy();
	void Destroy();
	bool Update(float dt);
	bool HandleInput();

	//Render fucntion
	void Draw(float dt);

	//Collision managment

	Collider* GetCollider() const;

protected:
	//Pathfinding functions
	virtual void Move(iPoint pos, float dt);
	virtual void LockOn(iPoint destiny, float dt);
	virtual bool CheckLockOn(iPoint destiny);
	virtual void AvoidStuck(iPoint destiny);
	virtual void CheckStuck();
	virtual void Pathfind(float dt);
	virtual void FollowPath(float dt);
	virtual void ResetPathCounter(float dt);

};

#endif // __ENEMY_H__
