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

	ST_FALLING,

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
	IN_FALLING,


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

	void Player_jump(player_states state);
	void Player_fall();
	void Check_if_falling();

public:
	int max_colliders_per_frame = 50;
	Collider *collider = nullptr;  //CHANGE IN XML  
	SDL_Texture* graphics = nullptr;
	SDL_Texture* bazooka = nullptr;
	bool godMode = false;

	SDL_Rect bazookaRect;
	SDL_Rect cursorRect;
	
	Animation walk;
	Animation idle;
	Animation jump;
	Animation rocketJump;
	Animation dead;


	uint fsx = 0u;

	iPoint playerBuffer;
	int speed;
	int jumpspeed;
	int speedcap;
	int grav;
	int buffer_jump_sign = 2; // we initiate it at an impossible number (sign only accepts +1 / 0 /-1) 
	int time_spent_jumping = 1;
	int time_spent_falling = 1;


	bool freeze = false;
	bool right = false;
	bool left = false;
	bool up = false;
	bool flip = false;

	player_states state;
	p2Qeue<player_inputs> inputs;
	iPoint	position;
	int cursorX, cursorY;

	p2SString	folder;
};

#endif
