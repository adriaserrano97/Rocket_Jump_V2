#ifndef __ModulePlayer1_H__
#define __ModulePlayer1_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2Qeue.h"

struct Mix_Chunk;

enum player_states
{
	ST_UNKNOWN,
	ST_IDLE,

	ST_WALKING_LEFT,
	ST_WALKING_RIGHT,

	ST_JUMP,
	ST_RIGHT_JUMP,
	ST_LEFT_JUMP,

	ST_ROCKET_JUMP,

	ST_DEAD
};

enum player_inputs
{
	IN_NEUTRAL,

	IN_LEFT_DOWN,
	IN_RIGHT_DOWN,
	IN_JUMP_DOWN,

	IN_LEFT_JUMP_DOWN,
	IN_RIGHT_JUMP_DOWN,	

	IN_LEFT_UP,
	IN_RIGHT_UP,
	IN_JUMP_FINISH,

	IN_SHOOT,
	IN_ROCKET_JUMP,

	IN_DEAD
};

struct SDL_Texture;


class j1Player : public j1Module
{
public:
	j1Player();
	~j1Player();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

	void internal_input(p2Qeue<player_inputs>& inputs);
	bool external_input(p2Qeue<player_inputs>& inputs);
	player_states process_fsm(p2Qeue<player_inputs>& inputs);
	void OnCollision(Collider* c1, Collider* c2);
	void BlitCharacterAndAddColliders(Animation* current_animation, SDL_Texture* texture);

	void ClearColliders();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	int max_colliders_per_frame = 50;
	Collider* colliders[50];  //CHANGE IN XML  
	SDL_Texture* graphics = nullptr;
	bool godMode = false;
	
	Animation walk;
	Animation idle;

	uint fsx = 0u;


	int speed;
	int grav = 1;

	bool freeze = false;
	bool right = false;
	bool left = false;
	bool up = false;
	
	bool flip = false;

	player_states state;
	p2Qeue<player_inputs> inputs;
	iPoint	position;

	p2SString	folder;
};

#endif
