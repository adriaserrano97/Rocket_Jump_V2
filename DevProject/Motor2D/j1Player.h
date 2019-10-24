#ifndef __ModulePlayer1_H__
#define __ModulePlayer1_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2Qeue.h"

struct Mix_Chunk;

enum PLAYER_STATES
{
	ST_UNKNOWN,
	ST_IDLE,

	ST_WALKING_LEFT,
	ST_WALKING_RIGHT,

	ST_JUMP,
	ST_RIGHT_JUMP,
	ST_LEFT_JUMP,

	ST_FALLING,

	ST_LEFT_ROCKET_JUMP,
	ST_RIGHT_ROCKET_JUMP,
	ST_UP_ROCKET_JUMP,
	ST_DOWN_ROCKET_JUMP,
	ST_LEFT_UP_ROCKET_JUMP,
	ST_LEFT_DOWN_ROCKET_JUMP,
	ST_RIGHT_UP_ROCKET_JUMP,
	ST_RIGHT_DOWN_ROCKET_JUMP,

	ST_DEAD
};

enum PLAYER_INPUTS
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

	IN_LEFT_ROCKET_JUMP,
	IN_RIGHT_ROCKET_JUMP,
	IN_UP_ROCKET_JUMP,
	IN_DOWN_ROCKET_JUMP,
	IN_LEFT_UP_ROCKET_JUMP,
	IN_LEFT_DOWN_ROCKET_JUMP,
	IN_RIGHT_UP_ROCKET_JUMP,
	IN_RIGHT_DOWN_ROCKET_JUMP,

	IN_DEAD,
	IN_ALIVE
};

enum COLLISION_WALL_DIRECTION
{
	DIRECTION_NONE = -1,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_MAX
};

enum COLLISION_EXPLOSION_DIRECTION
{
	R_DIRECTION_NONE = -1,
	R_DIRECTION_UP,
	R_DIRECTION_DOWN,
	R_DIRECTION_LEFT,
	R_DIRECTION_RIGHT,
	R_DIRECTION_LEFT_UP,
	R_DIRECTION_LEFT_DOWN,
	R_DIRECTION_RIGHT_UP,
	R_DIRECTION_RIGHT_DOWN,
	R_DIRECTION_MAX
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
	bool PostUpdate();
	bool CleanUp();

	void internal_input(p2Qeue<PLAYER_INPUTS>& inputs);
	bool external_input(p2Qeue<PLAYER_INPUTS>& inputs);
	PLAYER_STATES process_fsm(p2Qeue<PLAYER_INPUTS>& inputs);
	void OnCollision(Collider* c1, Collider* c2);
	void BlitCharacterAndAddColliders(Animation* current_animation, SDL_Texture* texture);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void playerJump(PLAYER_STATES state);
	void playerFall();
	bool ResetJumpCheck(SDL_Rect player, SDL_Rect collision);
	void Check_if_falling();
	
	void PlayerMov(float factor = 1); //if not told otherwise, does not alter any speed
	void Stay_in_map(SDL_Rect rect);

	COLLISION_WALL_DIRECTION checkDirection(SDL_Rect player, SDL_Rect collision);
	COLLISION_EXPLOSION_DIRECTION checkDirectionExplosion(SDL_Rect player, SDL_Rect collision);


public:
	int max_colliders_per_frame = 50;
	Collider *collider = nullptr;  
	SDL_Texture* graphics = nullptr;
	SDL_Texture* bazooka = nullptr;
	bool godMode = false;

	SDL_Rect bazookaRect;
	SDL_Rect cursorRect;
	
	Animation walk;
	Animation idle;
	Animation jump;
	Animation dead;

	Animation* current_animation;

	uint fsx = 0u;

	iPoint playerBuffer;
	int speed;
	int jumpspeed;
	int rocketJumpSpeed;
	int speedcap;
	int grav;
	int deadFall;
	int deadAnimation;
	int deadTimer;
	int explosion_CD;
	int JumpingDelta;
	int time_from_last_explosion;
	int deadTimerBuffer = 0;
	int buffer_jump_sign = 2; // we initiate it at an impossible number (sgn only accepts +1 / 0 /-1) 
	int time_spent_jumping = 1; //we always start our maps airborne
	int time_spent_falling = 1;
	float JumpAdjustMargin;

	bool freeze = false;
	bool right = false;
	bool left = false;
	bool up = false;
	bool flip = false;
	bool vertical;
	bool horizontal;
	

	PLAYER_STATES state;
	p2Qeue<PLAYER_INPUTS> inputs;
	iPoint	position;
	int cursorX, cursorY;

	p2SString	folder;
};

#endif
