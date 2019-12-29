#include "j1App.h"
#include "Player.h"
#include "j1EntityManager.h"
#include "j1Collision.h"
#include "j1Input.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Gui.h"

Player::Player(int x, int y)
{

	position.x = x;
	position.y = y;
	playerBuffer = { 0, 0 };

	to_delete = false;
	started = false;
	godMode = false;
	freeze = false;
	right = false;
	left = false;
	up = false;
	flip = false;
	vertical = false;

	texture = nullptr;
	bazooka = nullptr;

	deadTimerBuffer = 0.0f;
	buffer_jump_sign = 2; // we initiate it at an impossible number (sgn only accepts +1 / 0 /-1) 
	time_spent_jumping = 1.0f; //we always start our maps airborne
	time_spent_falling = 1.0f;
	cursorX = 0;
	cursorY = 0;

	collider = App->colliders->AddCollider({ x, y, App->entityManager->playerColRect.w, App->entityManager->playerColRect.h }, COLLIDER_PLAYER, (j1Module*)App->entityManager);;
	bazookaRect = App->entityManager->bazookaRect;
	cursorRect = App->entityManager->cursorRect;

	walk = App->entityManager->player_walk;
	idle = App->entityManager->player_idle;
	jump = App->entityManager->player_jump;
	dead = App->entityManager->player_dead;

	speed = App->entityManager->speed;
	jumpspeed = App->entityManager->jumpspeed;
	rocketJumpSpeed = App->entityManager->rocketJumpSpeed;
	speedcap = App->entityManager->speedcap;
	grav = App->entityManager->grav;
	deadFall = App->entityManager->deadFall;
	
	deadTimer = App->entityManager->deadTimer;
	explosion_CD = App->entityManager->explosion_CD;
	JumpingDelta = App->entityManager->JumpingDelta;
	JumpAdjustMargin = App->entityManager->JumpAdjustMargin;


	type = Entity::EntityTypes::PLAYER;

}

Player::~Player()
{
	//Unload all animations
	walk = Animation();
	idle = Animation();
	jump = Animation();
	dead = Animation();
	current_animation = nullptr;

	texture = nullptr;
	bazooka = nullptr;

	//Set all colliders to nullptr
	collider = nullptr;
}

bool Player::Start() {

	texture = App->entityManager->graphics;
	bazooka = App->entityManager->bazooka;
	inputs.Push(IN_ALIVE);

	return true;
}


// Update: draw background
bool Player::Update(float dt) {

	//get player's position before any changes are made to it
	playerBuffer = position;
	App->entityManager->playerPosition = position;

	//reset state box control variables
	vertical = false;
	
	//If no changes apply to player, this is its default state
	PLAYER_STATES current_state = ST_UNKNOWN;
	current_animation = &idle;

	//check inputs to traverse state matrix
	external_input(inputs, dt);
	internal_input(inputs, dt);
	state = process_fsm(inputs);


	if ((state != current_state) && !godMode && !freeze)
	{
		switch (state)
		{
		case ST_IDLE:
			current_animation = &idle;

			break;

		case ST_WALKING_LEFT:
			current_animation = &walk;
			position.x -= round(speed * dt);
			flip = true;
			break;

		case ST_WALKING_RIGHT:
			current_animation = &walk;
			position.x += round(speed * dt);
			flip = false;
			break;

		case ST_JUMP:
			current_animation = &jump;
			playerJump(ST_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			break;

		case ST_RIGHT_JUMP:
			current_animation = &jump;
			playerJump(ST_RIGHT_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			flip = false;
			break;

		case ST_LEFT_JUMP:
			current_animation = &jump;
			playerJump(ST_LEFT_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			flip = true;
			break;

		case ST_FALLING:
			current_animation = &jump;
			playerFall(dt);
			PlayerMov(dt, JumpAdjustMargin);
			break;

		case ST_LEFT_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_LEFT_ROCKET_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			break;

		case ST_RIGHT_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_RIGHT_ROCKET_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			break;

		case ST_UP_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_UP_ROCKET_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			break;

		case ST_DOWN_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_DOWN_ROCKET_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			break;

		case ST_LEFT_UP_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_LEFT_UP_ROCKET_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			break;

		case ST_LEFT_DOWN_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_LEFT_DOWN_ROCKET_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			break;

		case ST_RIGHT_UP_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_RIGHT_UP_ROCKET_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			break;

		case ST_RIGHT_DOWN_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_RIGHT_DOWN_ROCKET_JUMP, dt);
			PlayerMov(dt, JumpAdjustMargin);
			break;

		case ST_DEAD:
			current_animation = &dead;
			break;

		}
		position.y += round(grav * dt);
	}
	current_state = state;

	if (godMode && !freeze)
	{
		PlayerMov(dt);
	}

	//GOD MODE
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		if (godMode == false)
			godMode = true;
		else
			godMode = false;
	}

	// Set collider
	if (collider != nullptr)
	{
		collider->SetPos(position.x, position.y);
	}

	return true;
}


bool Player::external_input(p2Qeue<PLAYER_INPUTS>& inputs, float dt) {

	if (!godMode) {
		//Key UP

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) {
			up = true;
		}
		else
			up = false;


		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			right = true;
		else
			right = false;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			left = true;
		else
			left = false;


		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && round(time_from_last_explosion * dt) >= round(explosion_CD * dt)) { //This only creates one explosion, since the second frame transforms key_down in key_repeat
			time_from_last_explosion = 0;
			App->input->GetMousePosition(cursorX, cursorY);
			App->entityManager->CreateEntity(Entity::EntityTypes::EXPLOSION_PARTICLE, (cursorX - App->render->camera.x) - (App->entityManager->explosionAnimation.GetRect().w / 2), (cursorY - App->render->camera.y) - (App->entityManager->explosionAnimation.GetRect().h / 2));
			App->audio->PlayFx(App->audio->bomb_sound, 0);
		}


		if (right && !left) {

			if (up)
				inputs.Push(IN_RIGHT_JUMP_DOWN);


			else
				inputs.Push(IN_RIGHT_DOWN);
		}


		else if (!right && left) {

			if (up)
				inputs.Push(IN_LEFT_JUMP_DOWN);


			else
				inputs.Push(IN_LEFT_DOWN);
		}


		else if (up)
			inputs.Push(IN_JUMP_DOWN);


		if (!right)
			inputs.Push(IN_RIGHT_UP);


		if (!left)
			inputs.Push(IN_LEFT_UP);
	}
	return true;
}


void Player::internal_input(p2Qeue<PLAYER_INPUTS>& inputs, float dt) {

	if (time_from_last_explosion*dt < explosion_CD*dt) {
		time_from_last_explosion += dt;
	}
	if (position.y > deadFall && deadTimerBuffer == 0)
	{
		inputs.Push(IN_DEAD);
 		deadTimerBuffer += dt;
		App->gui->PlayerLifesCounter--;
		App->gui->UpdateLifesNCoins();

	}
	
	if (deadTimerBuffer > 0)
	{
		deadTimerBuffer += dt;
		if (deadTimerBuffer * dt >= deadTimer * dt)
		{
			deadTimerBuffer = 0;
			inputs.Push(IN_ALIVE);
			App->entityManager->Destroy_all();

			if (App->gui->PlayerLifesCounter > 0) {
				App->scene->load_from_save = true;
				App->scene->load_lifes_from_save = false;
				App->LoadGame();
			} 	
			if (App->gui->PlayerLifesCounter <= 0) {
				App->scene->playerNoLifes = true;
			}
		}
	}

	
}


PLAYER_STATES Player::process_fsm(p2Qeue<PLAYER_INPUTS>& inputs) {
	static PLAYER_STATES state = ST_IDLE;
	PLAYER_INPUTS last_input;

	while (inputs.Pop(last_input))
	{
		switch (state)
		{

		case ST_IDLE:
		{
			switch (last_input)
			{
			case IN_RIGHT_DOWN: state = ST_WALKING_RIGHT;	break;
			case IN_LEFT_DOWN: state = ST_WALKING_LEFT;		break;

			case IN_JUMP_DOWN: state = ST_JUMP;				break;
			case IN_LEFT_JUMP_DOWN: state = ST_LEFT_JUMP;	break;
			case IN_RIGHT_JUMP_DOWN: state = ST_RIGHT_JUMP; break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_FALLING: state = ST_FALLING;			break;

			case IN_DEAD: state = ST_DEAD;					break;

			}
		}	break;

		case ST_WALKING_RIGHT:
		{
			switch (last_input)
			{
			case IN_RIGHT_UP: state = ST_IDLE;				break;

			case IN_JUMP_DOWN: state = ST_RIGHT_JUMP;		break;
			case IN_RIGHT_JUMP_DOWN: state = ST_RIGHT_JUMP; break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_FALLING: state = ST_FALLING;			break;

			case IN_DEAD: state = ST_DEAD;					break;

			}
		}	break;

		case ST_WALKING_LEFT:
		{
			switch (last_input)
			{
			case IN_LEFT_UP: state = ST_IDLE;				break;

			case IN_JUMP_DOWN: state = ST_LEFT_JUMP;		break;
			case IN_LEFT_JUMP_DOWN: state = ST_LEFT_JUMP;	break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_FALLING: state = ST_FALLING;			break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_RIGHT_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_LEFT_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_LEFT_ROCKET_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_RIGHT_ROCKET_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_UP_ROCKET_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_DOWN_ROCKET_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_LEFT_UP_ROCKET_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_LEFT_DOWN_ROCKET_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_RIGHT_UP_ROCKET_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_RIGHT_DOWN_ROCKET_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_FALLING:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_LEFT_ROCKET_JUMP: state = ST_LEFT_ROCKET_JUMP;				break;
			case IN_RIGHT_ROCKET_JUMP: state = ST_RIGHT_ROCKET_JUMP;			break;
			case IN_UP_ROCKET_JUMP: state = ST_UP_ROCKET_JUMP;					break;
			case IN_DOWN_ROCKET_JUMP: state = ST_DOWN_ROCKET_JUMP;				break;
			case IN_LEFT_UP_ROCKET_JUMP: state = ST_LEFT_UP_ROCKET_JUMP;		break;
			case IN_LEFT_DOWN_ROCKET_JUMP: state = ST_LEFT_DOWN_ROCKET_JUMP;	break;
			case IN_RIGHT_UP_ROCKET_JUMP: state = ST_RIGHT_UP_ROCKET_JUMP;		break;
			case IN_RIGHT_DOWN_ROCKET_JUMP: state = ST_RIGHT_DOWN_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_DEAD:
		{
			switch (last_input)
			{
			case IN_ALIVE: state = ST_IDLE;					break;
			}

		}	break;

		}

	}

	return state;
}



//Player jump/movement functions

void Player::Check_if_falling() {
	//If player is mid air but did not jump, it means he is falling
	if (((abs(position.y)) > abs(playerBuffer.y)) && (time_spent_jumping == 0)) {
		inputs.Push(IN_FALLING);
	}
}

void Player::playerFall(float dt) {

	//Simulate acceleration
	int buffer_y = position.y;
	if (time_spent_falling == 0)
		time_spent_falling += dt;

	position.y += time_spent_falling;
	time_spent_falling += dt;

	//Adjust to max speed, prevent tunneling
	if ((abs(position.y) - abs(buffer_y)) > speedcap * dt) {
		position.y = (buffer_y + sgn(position.y)*speedcap * dt);
	}

}


void Player::playerJump(PLAYER_STATES state, float dt) {

	int buffer_y = position.y;
	//Check if player just jumped
	if (time_spent_jumping == 0) {
		time_spent_jumping += dt;
		//Play jump sound and add appropiate particles
		App->audio->PlayFx(App->audio->jump_sound, 0);
		App->entityManager->CreateEntity(Entity::EntityTypes::DUST_PARTICLE, position.x, playerBuffer.y);
	}

	switch (state) {
		//Remember, our reference system states that (0,0) is the upper left corner
	case ST_JUMP:
		position.y -= round(jumpspeed * dt);
		position.y += round(time_spent_jumping * dt);
		time_spent_jumping += dt;
		break;

	case ST_RIGHT_JUMP:
		position.y -= round(jumpspeed * dt);
		position.y += round(time_spent_jumping * dt);
		time_spent_jumping += dt;
		position.x += round(speed * dt);
		break;

	case ST_LEFT_JUMP:
		position.y -= round(jumpspeed * dt);
		position.y += round(time_spent_jumping * dt);
		time_spent_jumping += dt;
		position.x -= round(speed * dt);
		break;

	case ST_LEFT_ROCKET_JUMP:

		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.x -= round(rocketJumpSpeed * dt);
			position.x += round(time_spent_jumping * dt);
			time_spent_jumping += dt;
		}
		else
		{
			position.x -= round(speed * dt);
			playerFall(dt);
		}
		break;

	case ST_RIGHT_ROCKET_JUMP:
		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.x += round(rocketJumpSpeed * dt);
			position.x -= round(time_spent_jumping * dt);
			time_spent_jumping += dt;
		}
		else
		{
			position.x += round(speed * dt);
			playerFall(dt);
		}
		break;

	case ST_UP_ROCKET_JUMP:
		if (rocketJumpSpeed > time_spent_jumping) {
			position.y -= round(rocketJumpSpeed * dt);
			position.y += round(time_spent_jumping * dt);
			time_spent_jumping += dt;
		}
		else {
			playerFall(dt);
		}
		break;

	case ST_DOWN_ROCKET_JUMP:

		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.y += round(rocketJumpSpeed * dt);
			position.y -= round(time_spent_jumping * dt);
			time_spent_jumping += dt;
		}
		break;

	case ST_LEFT_UP_ROCKET_JUMP:
		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.y -= round(rocketJumpSpeed * dt);
			position.y += round(time_spent_jumping * dt);

			position.x -= round(rocketJumpSpeed * dt);
			position.x += round(time_spent_jumping * dt);
			time_spent_jumping += dt;
		}
		else
		{
			position.x -= round(speed * dt);
			playerFall(dt);
		}
		break;

		break;

	case ST_LEFT_DOWN_ROCKET_JUMP:
		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.y += round(rocketJumpSpeed * dt);
			position.y -= round(time_spent_jumping * dt);

			position.x -= round(rocketJumpSpeed * dt);
			position.x += round(time_spent_jumping * dt);
			time_spent_jumping += dt;
		}
		else
		{
			position.x -= round(speed * dt);
		}
		break;


	case ST_RIGHT_UP_ROCKET_JUMP:


		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.y -= round(rocketJumpSpeed * dt);
			position.y += round(time_spent_jumping * dt);

			position.x += round(rocketJumpSpeed * dt);
			position.x -= round(time_spent_jumping * dt);
			time_spent_jumping += dt;
		}
		else
		{
			position.x += round(round(speed * dt));
			playerFall(dt);
		}
		break;

	case ST_RIGHT_DOWN_ROCKET_JUMP:
		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.y += round(rocketJumpSpeed * dt);
			position.y -= round(time_spent_jumping * dt);

			position.x += round(rocketJumpSpeed * dt);
			position.x -= round(time_spent_jumping * dt);
			time_spent_jumping += dt;
		}
		break;

	default:
		LOG("Player state was not valid to perform a jump from");
		break;
	}

	// speed cap check, to prevent player from tunneling
	if ((abs(position.y) - abs(buffer_y)) > speedcap*dt) {
		position.y = buffer_y + sgn(position.y)*speedcap * dt;
	}

}

void Player::PlayerMov(float dt, float factor) {
	if (godMode && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) {
		position.y -= round(speed * factor * dt);
	}

	if (godMode && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		position.y += round(speed * factor * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		position.x += round(speed * factor * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		position.x -= round(speed * factor * dt);
	}
}

bool Player::ResetJumpCheck(SDL_Rect player, SDL_Rect collision) {

	int distance1;
	int distance2;
	distance1 = abs((player.x + player.w) - collision.x);
	distance2 = abs(player.x - (collision.x + collision.w));

	if (player.x < collision.x) { //left side

		if (abs((player.x + player.w) - collision.x) > JumpingDelta) {

			return true;
		}
		else {

			return false;
		}
	}
	else if (player.x > collision.x) { //right side

		if (abs(player.x - (collision.x + collision.w)) > JumpingDelta) {

			return true;
		}
		else {

			return false;
		}
	}

}

void Player::Stay_in_map(SDL_Rect rect) {

	//always adjust with 1 of diference so it the collision detction smoother


	//adjust left
	if (rect.x < 0) {
		position.x = 1;
	}
	//else if (rect.x < App->render->camera.x) {
		//position.x = App->render->camera.x;
	//}

	//right
	if ((rect.x + rect.w) > (App->map->data.width*App->map->data.tile_width)) {
		position.x = (App->map->data.width*App->map->data.tile_width) - (rect.w + 1);
	}
	//else if ((rect.x + rect.w) > (App->render->camera.x + App->render->camera.w )) {
	//position.x = (App->render->camera.x + App->render->camera.w) - rect.w -1;
	//}

	if (rect.y < 0) {
		position.y = 1;
	} //up

//if ((rect.y+rect.h) > (App->map->data.height*App->map->data.tile_height)) {
	//position.y = (App->map->data.height*App->map->data.tile_height) +  rect.h; } //down

	//we do not adjust down, since the player fallñing off the map is our death condition 
}


//collision management
void Player::OnCollision(Collider* c2) {

	if (c2->type == COLLIDER_WALL && godMode == false)
	{

		switch (checkDirection(collider->rect, c2->rect))
		{
		case DIRECTION_LEFT:
			if (!vertical)
			{
				position.x = c2->rect.x - collider->rect.w - 1;
				vertical = true;
			}

			break;

		case DIRECTION_RIGHT:
			if (!vertical)
			{
				position.x = c2->rect.x + c2->rect.w + 1;
				vertical = true;
			}

			break;

		case DIRECTION_UP:


			if (ResetJumpCheck(collider->rect, c2->rect) && (state == ST_JUMP || state == ST_LEFT_JUMP || state == ST_RIGHT_JUMP))
			{
				position.y = c2->rect.y - collider->rect.h - 1;
				//If colliding with a platform, stop jumping
				if (time_spent_jumping > 0 || time_spent_falling > 0) {


					time_spent_jumping = 0;
					time_spent_falling = 0;
					inputs.Push(IN_JUMP_FINISH);

				}


			}

			if ((state != ST_JUMP && state != ST_LEFT_JUMP && state != ST_RIGHT_JUMP))
			{
				position.y = c2->rect.y - collider->rect.h - 1;
				//If colliding with a platform, stop jumping
				if (time_spent_jumping > 0 || time_spent_falling > 0) {


					time_spent_jumping = 0;
					time_spent_falling = 0;
					inputs.Push(IN_JUMP_FINISH);

				}


			}

			break;

		case DIRECTION_DOWN:


			if (ResetJumpCheck(collider->rect, c2->rect) && (state == ST_JUMP || state == ST_LEFT_JUMP || state == ST_RIGHT_JUMP || state == ST_LEFT_ROCKET_JUMP ||
				state == ST_RIGHT_ROCKET_JUMP || state == ST_UP_ROCKET_JUMP || state == ST_DOWN_ROCKET_JUMP || state == ST_LEFT_UP_ROCKET_JUMP ||
				state == ST_LEFT_DOWN_ROCKET_JUMP || state == ST_RIGHT_UP_ROCKET_JUMP || state == ST_RIGHT_DOWN_ROCKET_JUMP))
			{
				position.y = c2->rect.y + c2->rect.h + 1;

			}

			if ((state != ST_JUMP && state != ST_LEFT_JUMP && state != ST_RIGHT_JUMP && state != ST_LEFT_ROCKET_JUMP &&
				state != ST_RIGHT_ROCKET_JUMP && state != ST_UP_ROCKET_JUMP && state != ST_DOWN_ROCKET_JUMP && state != ST_LEFT_UP_ROCKET_JUMP &&
				state != ST_LEFT_DOWN_ROCKET_JUMP && state != ST_RIGHT_UP_ROCKET_JUMP && state != ST_RIGHT_DOWN_ROCKET_JUMP))
			{
				position.y = c2->rect.y + c2->rect.h + 1;
			}

			break;
		}

	}

	//Ensure that upon transpassing a transpassable platform, player stops its movement above it
	if (c2->type == COLLIDER_TRANSPASSABLE_WALL && godMode == false)
	{
		if (position.y > playerBuffer.y)
		{
			if (checkDirection(collider->rect, c2->rect) == DIRECTION_UP)
			{
				position.y = c2->rect.y - collider->rect.h - 1;

				if (time_spent_jumping > 0 || time_spent_falling > 0) {
					time_spent_jumping = 0;
					time_spent_falling = 0;
					inputs.Push(IN_JUMP_FINISH);
				}
			}

		}
	}

	//Define the consequences of beign hit with a rocket explosion
	if (c2->type == COLLIDER_EXPLOSION && godMode == false)
	{

		//prevent player from building up speed
		time_spent_jumping = 0;
		time_spent_falling = 0;

		switch (checkDirectionExplosion(collider->rect, c2->rect))
		{
		case R_DIRECTION_LEFT:

			inputs.Push(IN_LEFT_ROCKET_JUMP);
			break;

		case R_DIRECTION_RIGHT:

			inputs.Push(IN_RIGHT_ROCKET_JUMP);
			break;

		case R_DIRECTION_UP:

			inputs.Push(IN_UP_ROCKET_JUMP);
			break;

		case R_DIRECTION_DOWN:

			inputs.Push(IN_DOWN_ROCKET_JUMP);
			break;

		case R_DIRECTION_LEFT_UP:

			inputs.Push(IN_LEFT_UP_ROCKET_JUMP);
			break;

		case R_DIRECTION_LEFT_DOWN:

			inputs.Push(IN_LEFT_DOWN_ROCKET_JUMP);
			break;

		case R_DIRECTION_RIGHT_UP:

			inputs.Push(IN_RIGHT_UP_ROCKET_JUMP);
			break;

		case R_DIRECTION_RIGHT_DOWN:

			inputs.Push(IN_RIGHT_DOWN_ROCKET_JUMP);
			break;
		}
	}


	if (c2->type == COLLIDER_ENEMY && godMode == false)
	{
		inputs.Push(IN_DEAD);
		deadTimerBuffer++;
	}	

	if (c2->type == COLLIDER_END_LEVEL && godMode == false)
	{
		godMode = true;
	}
}


//Check which side did player collide to
COLLISION_WALL_DIRECTION Player::checkDirection(SDL_Rect player, SDL_Rect collision) {

	int directionDiference[DIRECTION_MAX];

	directionDiference[DIRECTION_LEFT] = abs((playerBuffer.x + player.w) - collision.x);
	directionDiference[DIRECTION_RIGHT] = abs((collision.x + collision.w) - playerBuffer.x);
	directionDiference[DIRECTION_UP] = abs((playerBuffer.y + player.h) - collision.y);
	directionDiference[DIRECTION_DOWN] = abs((collision.y + collision.h) - playerBuffer.y);

	int directionCheck = DIRECTION_NONE;

	for (int i = 0; i < DIRECTION_MAX; ++i)
	{
		if (directionCheck == DIRECTION_NONE)
			directionCheck = i;
		else if ((directionDiference[i] < directionDiference[directionCheck]))
			directionCheck = i;

	}

	return (COLLISION_WALL_DIRECTION)directionCheck;
}


COLLISION_EXPLOSION_DIRECTION Player::checkDirectionExplosion(SDL_Rect player, SDL_Rect collision) {

	COLLISION_EXPLOSION_DIRECTION ret = R_DIRECTION_NONE;
	if (player.x <= collision.x + (int)(collision.w*0.3f))
	{
		if (player.y <= collision.y + (int)(collision.h*0.3f))
		{
			ret = R_DIRECTION_LEFT_UP;
		}

		if ((player.y < collision.y + (int)(collision.h*0.7f)) && (player.y > collision.y + (int)(collision.h*0.3f)))
		{
			ret = R_DIRECTION_LEFT;
		}

		if (player.y >= collision.y + (int)(collision.h*0.7f))
		{
			ret = R_DIRECTION_LEFT_DOWN;
		}

	}

	if ((player.x < collision.x + (int)(collision.w*0.7f)) && (player.x > collision.x + (int)(collision.w*0.3f)))
	{
		if (player.y <= collision.y + (int)(collision.h*0.3f))
		{
			ret = R_DIRECTION_UP;
		}

		if ((player.y < collision.y + (int)(collision.h*0.7f)) && (player.y > collision.y + (int)(collision.h*0.3f)))
		{
			ret = R_DIRECTION_UP;
		}

		if (player.y >= collision.y + (int)(collision.h*0.7))
		{
			ret = R_DIRECTION_DOWN;
		}
	}

	if (player.x >= collision.x + (int)(collision.w*0.7f))
	{
		if (player.y <= collision.y + (int)(collision.h*0.3f))
		{
			ret = R_DIRECTION_RIGHT_UP;
		}

		if ((player.y < collision.y + (int)(collision.h*0.7f)) && (player.y > collision.y + (int)(collision.h*0.3f)))
		{
			ret = R_DIRECTION_RIGHT;
		}

		if (player.y >= collision.y + (int)(collision.h*0.7f))
		{
			ret = R_DIRECTION_RIGHT_DOWN;
		}
	}

	return ret;
}


void Player::Draw(float dt) {

	//Assign definitive values to pos and collider
	if (collider != nullptr) {
		collider->SetPos(position.x, position.y);
		Stay_in_map(collider->rect);
	}

	//Once our positions are set, draw everything player related
	App->input->GetMousePosition(cursorX, cursorY);
	App->render->Blit(bazooka, position.x - bazookaRect.w / 2, position.y + bazookaRect.h / 2, &bazookaRect, NULL, NULL, NULL, NULL, flip);
	App->render->Blit(bazooka, (cursorX - cursorRect.w / 2) - App->render->camera.x, (cursorY - cursorRect.h / 2) - App->render->camera.y, &cursorRect);
	
	Frame r = current_animation->GetCurrentFrame(dt);

	App->render->Blit(texture, position.x, position.y, &r.frame, NULL, NULL, r.pivotPosition.x, r.pivotPosition.y, flip);

	//Once all changes are made, check if player is free falling
	Check_if_falling();
}

Collider* Player::GetCollider() const
{
	return collider;
}