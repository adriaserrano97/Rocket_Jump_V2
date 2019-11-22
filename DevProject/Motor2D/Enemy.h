#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "p2Point.h"
#include "Animation.h"

struct SDL_Texture;
struct Collider;

class Enemy
{
protected:
	//nope
public:
	iPoint position;
	iPoint position_buffer;
	Animation* animation = nullptr;
	Collider* collider = nullptr;

public:
	Enemy(int x, int y);
	virtual ~Enemy();

	const Collider* GetCollider() const;
	void Destroy();

	virtual void Move(iPoint pos, float dt) {};
	virtual void Draw(SDL_Texture* sprites, float dt);
	virtual void OnCollision(Collider* collider) {};
	virtual void AvoidStuck(iPoint destiny);
};

#endif // __ENEMY_H__
