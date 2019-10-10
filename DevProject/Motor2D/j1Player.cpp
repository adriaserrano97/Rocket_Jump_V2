#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Particles.h"
#include "j1Audio.h"
#include "j1Collision.h"
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


	walk = walk.PushPlayerAnimation(config, "run");
	idle = idle.PushPlayerAnimation(config, "idle");
	
	return ret;
}


// Load assets
bool j1Player::Start() {
	bool ret = true;

	graphics = App->tex->Load(PATH(folder.GetString(), "stickman_spritesheet.png"));
	
	
	
	return ret;
}

// Unload assets
bool j1Player::CleanUp() {

	LOG("Unloading Player 1");

	App->tex->UnLoad(graphics);

	walk = Animation();
	//Clear Colliders
	ClearColliders();

	return true;
}

bool j1Player::PreUpdate() {
	
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
			position.x = position.x - speed;
			flip = true;
			break;

		case ST_WALKING_RIGHT:
			position.x = position.x + speed;
			flip = false;
			break;

		case ST_JUMP:
			
			break;

		case ST_RIGHT_JUMP:
			break;

		case ST_LEFT_JUMP:
			break;

		case ST_ROCKET_JUMP:
			break;

		case ST_DEAD:
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

	BlitCharacterAndAddColliders(current_animation, graphics);

	return true;
}

void j1Player::ClearColliders() {

	for (int i = 0; i < max_colliders_per_frame; i++)
	{
		if (colliders[i] != nullptr) {
			colliders[i]->to_delete = true;
			colliders[i] = nullptr;
		}
	}
}


void j1Player::OnCollision(Collider* c1, Collider* c2) {

	
}

void j1Player::BlitCharacterAndAddColliders(Animation* current_animation, SDL_Texture* texture) {
	
	Frame frame = current_animation->GetCurrentFrame();
	SDL_Rect r;

	int hitboxesQnt = frame.GetColliderQnt();

	for (int i = 0; i < hitboxesQnt; i++)
	{
		r = frame.hitBoxeRects[i];
		if (frame.types[i] != COLLIDER_PLAYER)
			if (flip)
				colliders[i] = App->collider->AddCollider({ position.x - (r.w - frame.pivotPosition.x) + r.x, position.y - r.h + frame.pivotPosition.y + /*jumpHeight*/ - r.y ,r.w, r.h }, frame.types[i], frame.callbacks[i]);
			else
				colliders[i] = App->collider->AddCollider({ position.x - frame.pivotPosition.x - r.x, position.y - r.h + frame.pivotPosition.y /*+ jumpHeight*/ - r.y ,r.w, r.h }, frame.types[i], frame.callbacks[i]);
	}

	r = frame.frame;
	
	
	if (flip)
		App->render->Blit(texture, position.x - (r.w - frame.pivotPosition.x), position.y - r.h + frame.pivotPosition.y /*+ jumpHeight*/, &r, flip);
	else
		App->render->Blit(texture, position.x - frame.pivotPosition.x, position.y - r.h + frame.pivotPosition.y /*+ jumpHeight*/, &r, false);
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