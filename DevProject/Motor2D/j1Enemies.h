#ifndef __ModuleEnemies_H__
#define __ModuleEnemies_H__

#include "j1Module.h"
#include "AlienEnemy.h"
#include "WalkingEnemy.h"
#include "p2DynArray.h"
#include "j1Render.h" //we need access to lerp values and functions

class Enemy;


class j1Enemies : public j1Module
{
public:
	j1Enemies();
	~j1Enemies();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();


	//All the info we load from config
	p2SString	folder;
	SDL_Texture* spritesFlyAlien;
	SDL_Texture* spritesWalkAlien;
	int aggro_range;
	float delta_move; //defines when do enemies correct their pathfinding
	Animation alienAnimation;
	Animation walkingAlien;

	//List of existing enemies
	Enemy* enemies[MAX_ENEMIES]; 

	//Enemy generation
	void SpawnEnemy(ENEMY_TYPES type, int x, int y);
	
};

#endif // __ModuleEnemies_H__
