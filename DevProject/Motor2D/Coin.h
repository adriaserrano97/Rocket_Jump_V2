#ifndef __COIN_H__
#define __COIN_H__

#include "p2Point.h"
#include "p2DynArray.h"
#include "Entity.h"

struct SDL_Texture;
struct Collider;

class Coin: public Entity
{
public:

	Collider* collider;

public:
	Coin(int x, int y);
	~Coin();
	void Destroy();
	bool Start();
	bool Update(float dt);


	//Render fucntion
	void Draw(float dt);

	//Collision managment
	void OnCollision(Collider* collider);
	Collider* GetCollider() const;
};

#endif // __ENEMY_H__
