#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "Entity.h"
#include "p2Qeue.h"

struct SDL_Texture;
struct Collider;

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

class Player: public Entity
{
public:
	Player(int x, int y);
	~Player();

	bool Start();
	
	bool Update(float dt);

	void Draw(float dt);

	void internal_input(p2Qeue<PLAYER_INPUTS>& inputs, float dt);
	bool external_input(p2Qeue<PLAYER_INPUTS>& inputs, float dt);
	PLAYER_STATES process_fsm(p2Qeue<PLAYER_INPUTS>& inputs);


	void OnCollision(Collider* c2);
	COLLISION_WALL_DIRECTION checkDirection(SDL_Rect player, SDL_Rect collision);
	COLLISION_EXPLOSION_DIRECTION checkDirectionExplosion(SDL_Rect player, SDL_Rect collision);


	void playerJump(PLAYER_STATES state, float dt);
	void playerFall(float dt);
	bool ResetJumpCheck(SDL_Rect player, SDL_Rect collision);
	void Check_if_falling();

	void PlayerMov(float dt, float factor = 1); //if not told otherwise, does not alter any speed
	void Stay_in_map(SDL_Rect rect);

	Collider* GetCollider() const;

public:
	
	bool myflip = false;

	Collider* collider = nullptr;
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

	iPoint playerBuffer;
	int speed;
	int jumpspeed;
	int rocketJumpSpeed;
	int speedcap;
	int grav;
	int deadFall;
	int deadAnimationTime;
	int deadTimer;
	int explosion_CD;
	int JumpingDelta;
	float time_from_last_explosion;
	float deadTimerBuffer = 0;
	int buffer_jump_sign = 2; // we initiate it at an impossible number (sgn only accepts +1 / 0 /-1) 
	float time_spent_jumping = 1; //we always start our maps airborne
	float time_spent_falling = 1;
	float JumpAdjustMargin;

	bool freeze = false;
	bool right = false;
	bool left = false;
	bool up = false;
	bool flip = false;
	bool vertical;


	PLAYER_STATES state;
	p2Qeue<PLAYER_INPUTS> inputs;

	int cursorX, cursorY;


};




#endif  //__PLAYER_H__