#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Particles.h"
#include "j1Audio.h"
#include "p2Qeue.h"
#include "p2Log.h"

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


	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = 20;
	rect.h = 50;
	COLLIDER_TYPE type = COLLIDER_PLAYER;
	j1Module* callback = this;
	collider = App->colliders->AddCollider(rect, type, callback);

	walk = walk.PushPlayerAnimation(config, "run");
	idle = idle.PushPlayerAnimation(config, "idle");
	jump = jump.PushPlayerAnimation(config, "jump");

	
	bazookaRect.x = 0;
	bazookaRect.y = 0;
	bazookaRect.w = 70;
	bazookaRect.h = 18;

	cursorRect.x = 0;
	cursorRect.y = 20;
	cursorRect.w = 62;
	cursorRect.h = 62;

	
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
	//Clear Colliders
	ClearColliders();

	return true;
}


// Update: draw background
bool j1Player::Update(float dt) {

	player_states current_state = ST_UNKNOWN;
	Animation* current_animation = &walk;
	SDL_Texture *texture = graphics;
	

	external_input(inputs);
	internal_input(inputs);
	state = process_fsm(inputs);



	if (state != current_state)
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
			break;

		case ST_RIGHT_JUMP:
			current_animation = &jump;
			flip = false;
			break;

		case ST_LEFT_JUMP:
			current_animation = &jump;
			flip = true;
			break;

		case ST_ROCKET_JUMP:
			current_animation = &rocketJump;
			break;

		case ST_DEAD:
			current_animation = &dead;

			break;
		
		}
	}
	current_state = state;

	//GOD MODE
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		if (godMode == false)
			godMode = true;
		else
			godMode = false;
	}

	// Draw everything --------------------------------------	

	int cursorX = 0, cursorY;
	App->input->GetMousePosition(cursorX, cursorY);
	App->render->Blit(bazooka, position.x - bazookaRect.w / 2, position.y + bazookaRect.h / 2, &bazookaRect, NULL, NULL, NULL, NULL, flip);
	App->render->Blit(bazooka, (cursorX - cursorRect.w / 2) - App->render->camera.x, (cursorY - cursorRect.h / 2) - App->render->camera.y, &cursorRect);
	collider->SetPos(position.x, position.y);
	BlitCharacterAndAddColliders(current_animation, graphics);

	return true;
}

void j1Player::ClearColliders() {

		if (collider != nullptr) {
			collider->to_delete = true;
			collider = nullptr;
		}
	
}


void j1Player::OnCollision(Collider* c1, Collider* c2) {
	int z = 0;

	z++;
	
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

	if (!freeze) {
		//Key UP

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			up = true;
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

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_RIGHT_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_LEFT_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
			}

		}	break;

		case ST_ROCKET_JUMP:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE;			break;

			case IN_ROCKET_JUMP: state = ST_ROCKET_JUMP;	break;

			case IN_DEAD: state = ST_DEAD;					break;
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