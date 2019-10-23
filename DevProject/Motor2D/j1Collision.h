#ifndef __j1Collision_H__
#define __j1Collision_H__

#define MAX_COLLIDERS 400

#include "j1Module.h"
#include "p2Point.h"

enum COLLIDER_TYPE
{
	COLLIDER_NONE,
	COLLIDER_WALL,
	COLLIDER_TRANSPASSABLE_WALL, //we want untranspassable walls, so we make different colliders
	COLLIDER_PLAYER,
	COLLIDER_EXPLOSION,
	COLLIDER_END_LEVEL,
	COLLIDER_MAX
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	void SetPos(iPoint position)
	{
		rect.x = position.x;
		rect.y = position.y;
	}

	void SetPos(int posx, int posy) {
		rect.x = posx;
		rect.y = posy;
	}

	bool CheckCollision(const SDL_Rect& r) const;
};

class j1Collision : public j1Module
{
public:


	j1Collision();
	~j1Collision();

	bool Awake(pugi::xml_node&);
	bool Start();

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	Collider* AddCollider(Collider collider);
	void DebugDraw();
	void RemoveDeletedColliders();
	void CalculateCollisions();
	Collider* colliders[MAX_COLLIDERS];

private:

	
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug =  false;
};

#endif // __j1Collision_H__
