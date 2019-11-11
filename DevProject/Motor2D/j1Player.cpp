#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Particles.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "p2Qeue.h"
#include "p2Log.h"
#include "j1FadeToBlack.h"
#include "j1Render.h"
#include "j1Enemies.h"


j1Player::j1Player()
{
	name.create("player");
}

j1Player::~j1Player()
{}


bool j1Player::Awake(pugi::xml_node& config) {

	LOG("Loading player assets");
	bool ret = true;

	folder.create(config.child("folder").child_value());


	//player movement related variables
	startPos.x = position.x = config.child("playerData").attribute("initialX").as_int();
	startPos.y = position.y = config.child("playerData").attribute("initialY").as_int();
	
	speed = config.child("playerData").attribute("speed").as_int();
	jumpspeed = config.child("playerData").attribute("jumpspeed").as_int();
	rocketJumpSpeed = config.child("playerData").attribute("rocketJumpSpeed").as_int();
	speedcap = config.child("playerData").attribute("speedcap").as_int();
	grav = config.child("playerData").attribute("gravity").as_int();
	deadFall = config.child("playerData").attribute("deadFall").as_int(); 
	explosion_CD = config.child("playerData").attribute("explosion_CD").as_int();
	deadTimer = config.child("Animations").child("dead").attribute("time").as_int();
	JumpAdjustMargin = config.child("playerData").attribute("JumpAdjustMargin").as_float();
	JumpingDelta = config.child("playerData").attribute("JumpingDelta").as_int();

	//set counter time for our explosion CD
	time_from_last_explosion = explosion_CD;

	//player collider information
	SDL_Rect rect;
	rect.x = config.child("collider").attribute("rectX").as_int();
	rect.y = config.child("collider").attribute("rectY").as_int();
	rect.w = config.child("collider").attribute("rectW").as_int();
	rect.h = config.child("collider").attribute("rectH").as_int();
	COLLIDER_TYPE type = COLLIDER_PLAYER;
	j1Module* callback = this;
	collider = App->colliders->AddCollider(rect, type, callback);

	//Player animations, loaded from xml
	walk = walk.PushAnimation(config, "run");
	idle = idle.PushAnimation(config, "idle");
	jump = jump.PushAnimation(config, "jump");
	dead = dead.PushAnimation(config, "dead");

	
	//player's firearm position information
	bazookaRect.x = config.child("bazooka").attribute("x").as_int();
	bazookaRect.y = config.child("bazooka").attribute("y").as_int();
	bazookaRect.w = config.child("bazooka").attribute("w").as_int();
	bazookaRect.h = config.child("bazooka").attribute("h").as_int();

	//image used as cursor information
	cursorRect.x = config.child("cursor").attribute("x").as_int();
	cursorRect.y = config.child("cursor").attribute("y").as_int();
	cursorRect.w = config.child("cursor").attribute("w").as_int();
	cursorRect.h = config.child("cursor").attribute("h").as_int();

	

	return ret;
}

// Load assets
bool j1Player::Start() {
	bool ret = true;

	graphics = App->tex->Load(PATH(folder.GetString(), "stickman_spritesheet.png"));
	bazooka = App->tex->Load(PATH(folder.GetString(), "bazooka.png"));
	
	App->enemy->AddEnemy(ALIEN, 50, 150);

	return ret;
}

// Unload assets
bool j1Player::CleanUp() {

	LOG("Unloading Player");

	//Unload all textures
	App->tex->UnLoad(graphics);
	App->tex->UnLoad(bazooka);

	//Unload all animations
	walk = Animation();
	idle = Animation();
	jump = Animation();
	dead = Animation();
	current_animation = nullptr;

	//Set all colliders to nullptr
	if (collider != nullptr)
	{
		collider = nullptr;
	}
	


	return true;
}

bool j1Player::PreUpdate() {

	//get player's position before any changes are made to it
	playerBuffer = position; 

	//reset state box control variables
	vertical = false;

	return true;
}

// Update: draw background
bool j1Player::Update(float dt) {

	//If no changes apply to player, this is its default state
	PLAYER_STATES current_state = ST_UNKNOWN;
	current_animation = &idle;

	//get player spritesheet
	SDL_Texture *texture = graphics;
	
	//check inputs to traverse state matrix
	external_input(inputs);
	internal_input(inputs);
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
			position.x = position.x - speed;
			flip = true;
			break;

		case ST_WALKING_RIGHT:
			current_animation = &walk;
			position.x = position.x + speed;
			flip = false;
			break;

		case ST_JUMP:
			current_animation = &jump;
			playerJump(ST_JUMP);
			PlayerMov(JumpAdjustMargin);
			break;

		case ST_RIGHT_JUMP:
			current_animation = &jump;
			playerJump(ST_RIGHT_JUMP);
			PlayerMov(JumpAdjustMargin);
			flip = false;
			break;

		case ST_LEFT_JUMP:
			current_animation = &jump;
			playerJump(ST_LEFT_JUMP);
			PlayerMov(JumpAdjustMargin);
			flip = true;
			break;

		case ST_FALLING:
			current_animation = &jump;
			playerFall();
			break;

		case ST_LEFT_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_LEFT_ROCKET_JUMP);
			PlayerMov(JumpAdjustMargin);
			break;

		case ST_RIGHT_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_RIGHT_ROCKET_JUMP);
			PlayerMov(JumpAdjustMargin);
			break;

		case ST_UP_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_UP_ROCKET_JUMP);
			PlayerMov(JumpAdjustMargin);
			break;

		case ST_DOWN_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_DOWN_ROCKET_JUMP);
			PlayerMov(JumpAdjustMargin);
			break;

		case ST_LEFT_UP_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_LEFT_UP_ROCKET_JUMP);
			PlayerMov(JumpAdjustMargin);
			break;

		case ST_LEFT_DOWN_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_LEFT_DOWN_ROCKET_JUMP);
			PlayerMov(JumpAdjustMargin);
			break;

		case ST_RIGHT_UP_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_RIGHT_UP_ROCKET_JUMP);
			PlayerMov(JumpAdjustMargin);
			break;

		case ST_RIGHT_DOWN_ROCKET_JUMP:
			current_animation = &idle;
			playerJump(ST_RIGHT_DOWN_ROCKET_JUMP);
			PlayerMov(JumpAdjustMargin);
			break;

		case ST_DEAD:
			current_animation = &dead;
			break;
		
		}
		position.y += grav;
	}
	current_state = state;

	if (godMode && !freeze)
	{
		PlayerMov();
	}

	//GOD MODE
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		if (godMode == false)
			godMode = true;
		else
			godMode = false;
	}	
	// Set collider

	//Check that player is not moving out of map
	

	collider->SetPos(position.x, position.y);
	
	return true;
}

bool j1Player::PostUpdate() {

	//Assign definitive values to pos and collider
	collider->SetPos(position.x, position.y);
	Stay_in_map(collider->rect);


	//Once our positions are set, draw everything player related
	App->input->GetMousePosition(cursorX, cursorY);
	App->render->Blit(bazooka, position.x - bazookaRect.w / 2, position.y + bazookaRect.h / 2, &bazookaRect, NULL, NULL, NULL, NULL, flip);
	App->render->Blit(bazooka, (cursorX - cursorRect.w / 2) - App->render->camera.x , (cursorY - cursorRect.h / 2) - App->render->camera.y, &cursorRect);
	BlitCharacterAndAddColliders(current_animation, graphics);

	//Once all changes are made, check if player is free falling
	Check_if_falling();

	return true;
}


void j1Player::OnCollision(Collider* c1, Collider* c2) {

	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_WALL && godMode == false)
	{

		switch (checkDirection(c1->rect, c2->rect))
		{
		case DIRECTION_LEFT:
			if (!vertical)
			{
				position.x = c2->rect.x - c1->rect.w - 1;
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
			
			
			if (ResetJumpCheck(c1->rect, c2->rect) && (state == ST_JUMP || state == ST_LEFT_JUMP || state == ST_RIGHT_JUMP))
			{
				position.y = c2->rect.y - c1->rect.h - 1;
				//If colliding with a platform, stop jumping
				if (time_spent_jumping > 0 || time_spent_falling > 0) {
					
					
					time_spent_jumping = 0;
					time_spent_falling = 0;
					inputs.Push(IN_JUMP_FINISH);
			
				}

				
			}

			if ((state != ST_JUMP && state != ST_LEFT_JUMP && state != ST_RIGHT_JUMP))
			{
				position.y = c2->rect.y - c1->rect.h - 1;
				//If colliding with a platform, stop jumping
				if (time_spent_jumping > 0 || time_spent_falling > 0) {


					time_spent_jumping = 0;
					time_spent_falling = 0;
					inputs.Push(IN_JUMP_FINISH);

				}

				
			}
			
			break;

		case DIRECTION_DOWN:
				

				if (ResetJumpCheck(c1->rect, c2->rect) && (state == ST_JUMP || state == ST_LEFT_JUMP || state == ST_RIGHT_JUMP || state == ST_LEFT_ROCKET_JUMP || 
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
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_TRANSPASSABLE_WALL && godMode == false)
	{
		if (position.y > playerBuffer.y)
		{
			if (checkDirection(c1->rect, c2->rect) == DIRECTION_UP)
			{
				position.y = c2->rect.y - c1->rect.h - 1;

				if (time_spent_jumping > 0 || time_spent_falling > 0) {
					time_spent_jumping = 0;
					time_spent_falling = 0;
					inputs.Push(IN_JUMP_FINISH);
				}
			}
			
		}
	}

	//Define the consequences of beign hit with a rocket explosion
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_EXPLOSION && godMode == false)
	{
			
		//prevent player from building up speed
		time_spent_jumping = 0;
		time_spent_falling = 0;

		switch (checkDirectionExplosion(c1->rect, c2->rect))
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


	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_ENEMY)
	{
		inputs.Push(IN_DEAD);
		deadTimerBuffer++;
	}
}

void j1Player::BlitCharacterAndAddColliders(Animation* current_animation, SDL_Texture* texture) {
	
	Frame frame = current_animation->GetCurrentFrame();
	SDL_Rect r;

	r = frame.frame;
	
	App->render->Blit(texture, position.x, position.y, &r, NULL, NULL, frame.pivotPosition.x, frame.pivotPosition.y, flip);

}

bool j1Player::external_input(p2Qeue<PLAYER_INPUTS>& inputs) {

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


		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && time_from_last_explosion==explosion_CD) { //This only creates one explosion, since the second frame transforms key_down in key_repeat
			time_from_last_explosion = 0;
			App->input->GetMousePosition(cursorX, cursorY);
			App->particles->AddParticle(App->particles->explosion, false, (cursorX - App->render->camera.x) - (App->particles->explosion.anim.frames->frame.w/2), (cursorY - App->render->camera.y) - (App->particles->explosion.anim.frames->frame.h / 2), 0, 0, COLLIDER_EXPLOSION, 0, 0);
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

void j1Player::internal_input(p2Qeue<PLAYER_INPUTS>& inputs) {
	
	if (position.y > deadFall && deadTimerBuffer == 0)
	{
		inputs.Push(IN_DEAD);
		deadTimerBuffer++;
	}

	if (deadTimerBuffer > 0)
	{
		deadTimerBuffer++;
		if (deadTimerBuffer == deadTimer)
		{
			deadTimerBuffer = 0;
			inputs.Push(IN_ALIVE);
			App->LoadGame(); //go back to last checkpoint	
		}
	}

	if (time_from_last_explosion < explosion_CD) {
		time_from_last_explosion++;
	}
}

PLAYER_STATES j1Player::process_fsm(p2Qeue<PLAYER_INPUTS>& inputs) {
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

// Define where does player spawn
bool j1Player::Load(pugi::xml_node& data)
{
	position.x = data.attribute("x").as_int();
	position.y = data.attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Player::Save(pugi::xml_node& data) const
{
	
	data.append_attribute("x") = position.x;
	data.append_attribute("y") = position.y;

	return true;
}

//Player jump/movement functions

void j1Player::Check_if_falling() { 
	//If player is mid air but did not jump, it means he is falling
	if (((abs(position.y)) > abs(playerBuffer.y)) && (time_spent_jumping == 0)) { 
		inputs.Push(IN_FALLING); 
	}	
}

void j1Player::playerFall() { 

	//Simulate acceleration
	int buffer_y = position.y;
	if (time_spent_falling == 0) { time_spent_falling++; }
	position.y += time_spent_falling;
	time_spent_falling++;

	//Adjust to max speed, prevent tunneling
	if ((abs(position.y) - abs(buffer_y)) > speedcap) {
		position.y = buffer_y + sgn(position.y)*speedcap;
	}

 }

void j1Player::playerJump(PLAYER_STATES state) {

	int buffer_y = position.y;
	//Check if player just jumped
	if (time_spent_jumping == 0) {
		time_spent_jumping++; 
		//Play jump sound and add appropiate particles
		App->audio->PlayFx(App->audio->jump_sound, 0);
		App->particles->AddParticle(App->particles->dust, false, position.x, position.y + collider->rect.h, 0, 0, COLLIDER_NONE, 0, 0);
	}

	switch (state) {
		//Remember, our reference system states that (0,0) is the upper left corner
	case ST_JUMP:
		position.y -= jumpspeed;  
		position.y += time_spent_jumping;
		time_spent_jumping++;
		break;

	case ST_RIGHT_JUMP:
		position.y -= jumpspeed;
		position.y += time_spent_jumping;
		time_spent_jumping++;
		position.x += speed;
		break;

	case ST_LEFT_JUMP:
		position.y -= jumpspeed;
		position.y += time_spent_jumping;
		time_spent_jumping++;
		position.x -= speed;
		break;

	case ST_LEFT_ROCKET_JUMP:
		
		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.x -= rocketJumpSpeed;
			position.x += time_spent_jumping;
			time_spent_jumping++;
		}
		else
		{
			position.x -= speed;
			playerFall();
		}
		break;

	case ST_RIGHT_ROCKET_JUMP:
		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.x += rocketJumpSpeed;
			position.x -= time_spent_jumping;
			time_spent_jumping++;
		}
		else
		{
			position.x += speed;
			playerFall();
		}
		break;

	case ST_UP_ROCKET_JUMP:
		if (rocketJumpSpeed > time_spent_jumping) {
			position.y -= rocketJumpSpeed;
			position.y += time_spent_jumping;
			time_spent_jumping++;
		}
		else{
		playerFall();
		}
		break;

	case ST_DOWN_ROCKET_JUMP:
		
		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.y += rocketJumpSpeed;
			position.y -= time_spent_jumping;
			time_spent_jumping++;
		}
		break;

	case ST_LEFT_UP_ROCKET_JUMP:
		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.y -= rocketJumpSpeed;
			position.y += time_spent_jumping;

			position.x -= rocketJumpSpeed;
			position.x += time_spent_jumping;
			time_spent_jumping++;
		}
		else
		{
			position.x -= speed;
			playerFall();
		}
		break;

		break;

	case ST_LEFT_DOWN_ROCKET_JUMP:
		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.y += rocketJumpSpeed;
			position.y -= time_spent_jumping;

			position.x -= rocketJumpSpeed;
			position.x += time_spent_jumping;
			time_spent_jumping++;
		}
		else
		{
			position.x -= speed;
		}
		break;


	case ST_RIGHT_UP_ROCKET_JUMP:


		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.y -= rocketJumpSpeed;
			position.y += time_spent_jumping;

			position.x += rocketJumpSpeed;
			position.x -= time_spent_jumping;
			time_spent_jumping++;
		}
		else
		{
			position.x += speed;
			playerFall();
		}
		break;

	case ST_RIGHT_DOWN_ROCKET_JUMP:
		if (rocketJumpSpeed > time_spent_jumping)
		{
			position.y += rocketJumpSpeed;
			position.y -= time_spent_jumping;

			position.x += rocketJumpSpeed;
			position.x -= time_spent_jumping;
			time_spent_jumping++;
		}
		break;

	default:
		LOG("Player state was not valid to perform a jump from");
		break;
	}

	// speed cap check, to prevent player from tunneling
	if ((abs(position.y) - abs(buffer_y)) > speedcap) {
		position.y = buffer_y + sgn(position.y)*speedcap;
	}

}

void j1Player::PlayerMov( float factor) {
	if (godMode && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) {
		position.y -= (int)(speed *factor); 
	}

	if (godMode && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		position.y += (int)(speed * factor);
	}

 	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		position.x += (int)(speed * factor);
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		position.x -= (int)(speed * factor);
	}
}

bool j1Player::ResetJumpCheck(SDL_Rect player, SDL_Rect collision) {

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

void j1Player::Stay_in_map(SDL_Rect rect) {
	
	//always adjust with 1 of diference so it the collision detction smoother

	if (rect.x < 0) { 
		position.x = 1;} //adjust left
	
	
	if ((rect.x+rect.w) > (App->map->data.width*App->map->data.tile_width)) { 
		position.x = (App->map->data.width*App->map->data.tile_width) - (rect.w + 1); } //right

	if (rect.y < 0) { 
		position.y = 1; } //up
	
	//if ((rect.y+rect.h) > (App->map->data.height*App->map->data.tile_height)) {
		//position.y = (App->map->data.height*App->map->data.tile_height) +  rect.h; } //down
		
		//we do not adjust down, since the player fallñing off the map is our death condition 
}

//Check which side did player collide to
COLLISION_WALL_DIRECTION j1Player::checkDirection(SDL_Rect player, SDL_Rect collision) {

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


COLLISION_EXPLOSION_DIRECTION j1Player::checkDirectionExplosion(SDL_Rect player, SDL_Rect collision) {

	COLLISION_EXPLOSION_DIRECTION ret = R_DIRECTION_NONE;
	if (player.x <= collision.x + (int)(collision.w*0.3))
	{
		if (player.y <= collision.y + (int)(collision.h*0.3))
		{
			ret = R_DIRECTION_LEFT_UP;
		}

		if ((player.y < collision.y + (int)(collision.h*0.7)) && (player.y > collision.y + (int)(collision.h*0.3)))
		{
			ret = R_DIRECTION_LEFT;
		}

		if (player.y >= collision.y + (int)(collision.h*0.7))
		{
			ret = R_DIRECTION_LEFT_DOWN;
		}

	}

	if ((player.x < collision.x + (int)(collision.w*0.7)) && (player.x > collision.x + (int)(collision.w*0.3)))
	{
		if (player.y <= collision.y + (int)(collision.h*0.3))
		{
			ret = R_DIRECTION_UP;
		}

		if ((player.y < collision.y + (int)(collision.h*0.7)) && (player.y > collision.y + (int)(collision.h*0.3)))
		{
			ret = R_DIRECTION_UP;
		}

		if (player.y >= collision.y + (int)(collision.h*0.7))
		{
			ret = R_DIRECTION_DOWN;
		}
	}

	if (player.x >= collision.x + (int)(collision.w*0.7))
	{
		if (player.y <= collision.y + (int)(collision.h*0.3))
		{
			ret = R_DIRECTION_RIGHT_UP;
		}

		if ((player.y < collision.y + (int)(collision.h*0.7)) && (player.y > collision.y + (int)(collision.h*0.3)))
		{
			ret = R_DIRECTION_RIGHT;
		}

		if (player.y >= collision.y + (int)(collision.h*0.7))
		{
			ret = R_DIRECTION_RIGHT_DOWN;
		}
	}

	return ret;
}
