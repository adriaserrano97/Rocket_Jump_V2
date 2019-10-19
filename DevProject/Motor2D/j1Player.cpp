#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Particles.h"
#include "j1Audio.h"
#include "p2Qeue.h"
#include "p2Log.h"
#include "j1FadeToBlack.h"

#include "SDL\include\SDL.h"


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

	position.x = config.child("playerData").attribute("initialX").as_int();
	position.y = config.child("playerData").attribute("initialY").as_int();
	speed = config.child("playerData").attribute("speed").as_int();
	jumpspeed = config.child("playerData").attribute("jumpspeed").as_int();
	speedcap = config.child("playerData").attribute("speedcap").as_int();
	grav = config.child("playerData").attribute("gravity").as_int();
	deadFall = config.child("playerData").attribute("deadFall").as_int();

	SDL_Rect rect;
	rect.x = config.child("collider").attribute("rectX").as_int();
	rect.y = config.child("collider").attribute("rectY").as_int();
	rect.w = config.child("collider").attribute("rectW").as_int();
	rect.h = config.child("collider").attribute("rectH").as_int();
	COLLIDER_TYPE type = COLLIDER_PLAYER;
	j1Module* callback = this;
	collider = App->colliders->AddCollider(rect, type, callback);

	walk = walk.PushAnimation(config, "run");
	idle = idle.PushAnimation(config, "idle");
	jump = jump.PushAnimation(config, "jump");
	dead = dead.PushAnimation(config, "dead");
	deadTimer = config.child("Animations").child("dead").attribute("time").as_int();
	
	bazookaRect.x = config.child("bazooka").attribute("x").as_int();
	bazookaRect.y = config.child("bazooka").attribute("y").as_int();
	bazookaRect.w = config.child("bazooka").attribute("w").as_int();
	bazookaRect.h = config.child("bazooka").attribute("h").as_int();

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
	
	return ret;
}

// Unload assets
bool j1Player::CleanUp() {

	LOG("Unloading Player");

	App->tex->UnLoad(graphics);
	App->tex->UnLoad(bazooka);

	walk = Animation();
	idle = Animation();
	jump = Animation();
	rocketJump = Animation();
	dead = Animation();

	current_animation = nullptr;

	collider->to_delete = true;
	collider = nullptr;

	return true;
}

bool j1Player::PreUpdate() {
	playerBuffer = position;
	vertical = false;
	horizontal = false;
	return true;
}

// Update: draw background
bool j1Player::Update(float dt) {

	//this is the default state of the player. Changes apply below

	player_states current_state = ST_UNKNOWN;
	current_animation = &idle;

	SDL_Texture *texture = graphics;
	

	external_input(inputs);
	internal_input(inputs);
	state = process_fsm(inputs);


	if ((state != current_state) && godMode == false)
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
			Player_jump(ST_JUMP);
			break;

		case ST_RIGHT_JUMP:
			current_animation = &jump;
			Player_jump(ST_RIGHT_JUMP);
			flip = false;
			break;

		case ST_LEFT_JUMP:
			current_animation = &jump;
			Player_jump(ST_LEFT_JUMP);
			flip = true;
			break;

		case ST_FALLING:
			current_animation = &jump;
			Player_fall();
			break;

		case ST_ROCKET_JUMP:
			current_animation = &rocketJump;
			break;

		case ST_DEAD:
			current_animation = &dead;
			break;
		
		}
		position.y += grav;
	}
	current_state = state;

	if (godMode)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			position.y -= 5;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			position.y += 5;
		}
		
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			position.x += 5;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			position.x -= 5;
		}
	}

	//GOD MODE
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		if (godMode == false)
			godMode = true;
		else
			godMode = false;
	}

	// Draw everything --------------------------------------	

	collider->SetPos(position.x, position.y);
	
	return true;
}

bool j1Player::PostUpdate() {

	App->input->GetMousePosition(cursorX, cursorY);
	App->render->Blit(bazooka, position.x - bazookaRect.w / 2, position.y + bazookaRect.h / 2, &bazookaRect, NULL, NULL, NULL, NULL, flip);
	App->render->Blit(bazooka, (cursorX - cursorRect.w / 2) - App->render->camera.x , (cursorY - cursorRect.h / 2) - App->render->camera.y, &cursorRect);
	collider->SetPos(position.x, position.y);
	BlitCharacterAndAddColliders(current_animation, graphics);

	Check_if_falling();

	return true;
}


void j1Player::OnCollision(Collider* c1, Collider* c2) {

	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_WALL)
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
			if (!horizontal)
			{
				position.y = c2->rect.y - c1->rect.h - 1;

				if (time_spent_jumping > 0 || time_spent_falling > 0) {
					time_spent_jumping = 0;
					time_spent_falling = 0;
					inputs.Push(IN_JUMP_FINISH);
				}

				horizontal = true;
			}
			
			break;

		case DIRECTION_DOWN:
			if (!horizontal)
			{
				position.y = c2->rect.y + c2->rect.h + 1;
				horizontal = true;
			}
		

			break;
		}

	}
}

void j1Player::BlitCharacterAndAddColliders(Animation* current_animation, SDL_Texture* texture) {
	
	Frame frame = current_animation->GetCurrentFrame();
	SDL_Rect r;

	r = frame.frame;
	
	
	if (flip)
		App->render->Blit(texture, position.x, position.y   /*+ jumpHeight*/, &r, NULL, NULL, frame.pivotPosition.x, frame.pivotPosition.y, flip);
	else
		App->render->Blit(texture, position.x, position.y /*+ jumpHeight*/, &r, NULL, NULL, frame.pivotPosition.x, frame.pivotPosition.y, false);
}

bool j1Player::external_input(p2Qeue<player_inputs>& inputs) {

	if (!godMode) {
		//Key UP

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
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


		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) { //ADRI: this only creates one explosion, since the second frame r is key_down it becomes key_repeat
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

void j1Player::internal_input(p2Qeue<player_inputs>& inputs) {
	
	if (position.y > deadFall && deadTimerBuffer == 0)
	{
		inputs.Push(IN_DEAD);
		//App->fade->FadeToBlack(2);
		deadTimerBuffer++;
	}

	if (deadTimerBuffer > 0)
	{
		deadTimerBuffer++;
		if (deadTimerBuffer == deadTimer)
		{
			deadTimerBuffer = 0;
			inputs.Push(IN_ALIVE);
			App->LoadGame();
			
		}
	}

}

player_states j1Player::process_fsm(p2Qeue<player_inputs>& inputs) {
	static player_states state = ST_IDLE;
	player_inputs last_input;

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

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

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

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

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

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

			case IN_FALLING: state = ST_FALLING;			break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

			//case IN_FALLING: state = ST_FALLING;			break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_RIGHT_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

			//case IN_FALLING: state = ST_FALLING;			break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_LEFT_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

			//case IN_FALLING: state = ST_FALLING;			break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_ROCKET_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

			//case IN_FALLING: state = ST_FALLING;			break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;


		case ST_FALLING:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

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


bool j1Player::Load(pugi::xml_node& data)
{
	position.x = data.child("player").attribute("x").as_int();
	position.y = data.child("player").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Player::Save(pugi::xml_node& data) const
{
	pugi::xml_node ply = data.append_child("player");

	ply.append_attribute("x") = position.x;
	ply.append_attribute("y") = position.y;

	return true;
}


//Player jump functions


//TODO ADRI: ASK MARC ABOUT THIS FUCKING BLACK MAGIC

void j1Player::Check_if_falling() { //TRY PUTTING HERE AN INTERRUPTION POINT
	if (((abs(position.y)) > abs(playerBuffer.y)) && (time_spent_jumping == 0)) { //AND NOW, TRY HERE
		inputs.Push(IN_FALLING); 
	}	
}
void j1Player::Player_fall() { 

	int buffer_y = position.y;
	if (time_spent_falling == 0) { time_spent_falling++; }

	position.y += time_spent_falling;
	time_spent_falling++;

	if ((abs(position.y) - abs(buffer_y)) > speedcap) {

		position.y = buffer_y + sgn(position.y)*speedcap;
	}

 }

void j1Player::Player_jump(player_states state) {

	int buffer_y = position.y;
	if (time_spent_jumping == 0) {
		time_spent_jumping++; App->audio->PlayFx(App->audio->jump_sound, 0);
	}
	switch (state) {

	case ST_JUMP:
		position.y -= jumpspeed;  //Remember, our reference system states that (0,0) is the upper left corner
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

	default:
		LOG("Player state was not valid to perform a jump from");
		break;
	}
	// speed cap prevents player from avoiding colliders at high speed
	if ((abs(position.y) - abs(buffer_y)) > speedcap) {

		position.y = buffer_y + sgn(position.y)*speedcap;

	}

}

collision_direction j1Player::checkDirection(SDL_Rect player, SDL_Rect collision) {

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

	return (collision_direction)directionCheck;
}