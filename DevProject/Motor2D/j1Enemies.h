#ifndef __ModuleEnemies_H__
#define __ModuleEnemies_H__

#include "j1Module.h"
#include "AlienEnemy.h"
#include "WalkingEnemy.h"
#include "p2DynArray.h"
#include "j1Render.h" //we need access to lerp values and functions

#define MAX_ENEMIES 100

enum ENEMY_TYPES
{
	NO_TYPE,
	ALIEN,
	WALKING_ALIEN
};

enum COLLISION_WALL_DIRECTION;

class Enemy;

struct EnemyInfo
{
	ENEMY_TYPES type = ENEMY_TYPES::NO_TYPE;
	int x, y;
	bool in_path = false;
	int frames_stuck = 0;
	p2DynArray<iPoint>* path = nullptr;
	
};

class j1Enemies : public j1Module
{
public:

	int aggro_range;
	float delta_move; //defines when do enemies correct their pathfinding

public:

	j1Enemies();
	~j1Enemies();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);
	bool Save(pugi::xml_node&) const;

	bool AddEnemy(ENEMY_TYPES type, int x, int y);

	Animation alienAnimation;
	Animation walkingAlien;

	COLLISION_WALL_DIRECTION checkDirection(SDL_Rect enemy, SDL_Rect collision);
private:

	void SpawnEnemy(const EnemyInfo& info);

	

private:

	EnemyInfo queue[MAX_ENEMIES];
	Enemy* enemies[MAX_ENEMIES];

	SDL_Texture* spritesFlyAlien;
	SDL_Texture* spritesWalkAlien;

	p2SString	folder;
};

#endif // __ModuleEnemies_H__
