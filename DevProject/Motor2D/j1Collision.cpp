#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Scene.h"


j1Collision::j1Collision()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	name.create("colliders");
}

// Destructor
j1Collision::~j1Collision()
{}

bool j1Collision::Awake(pugi::xml_node& node) {

	LOG("Preparinc collision module");
	bool ret = true;

	//Here goes the matrix of collider interactions
	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_TRANSPASSABLE_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_WALL][COLLIDER_EXPLOSION] = false;
	matrix[COLLIDER_WALL][COLLIDER_END_LEVEL] = false;


	matrix[COLLIDER_TRANSPASSABLE_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_TRANSPASSABLE_WALL][COLLIDER_TRANSPASSABLE_WALL] = false;
	matrix[COLLIDER_TRANSPASSABLE_WALL][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_TRANSPASSABLE_WALL][COLLIDER_EXPLOSION] = false;
	matrix[COLLIDER_TRANSPASSABLE_WALL][COLLIDER_END_LEVEL] = false;
	

	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_TRANSPASSABLE_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_EXPLOSION] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_END_LEVEL] = false;


	matrix[COLLIDER_EXPLOSION][COLLIDER_WALL] = false;
	matrix[COLLIDER_EXPLOSION][COLLIDER_TRANSPASSABLE_WALL] = false;
	matrix[COLLIDER_EXPLOSION][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_EXPLOSION][COLLIDER_EXPLOSION] = false;
	matrix[COLLIDER_EXPLOSION][COLLIDER_END_LEVEL] = false;

	matrix[COLLIDER_END_LEVEL][COLLIDER_WALL] = false;
	matrix[COLLIDER_END_LEVEL][COLLIDER_TRANSPASSABLE_WALL] = false;
	matrix[COLLIDER_END_LEVEL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_END_LEVEL][COLLIDER_EXPLOSION] = false;
	matrix[COLLIDER_END_LEVEL][COLLIDER_END_LEVEL] = false;

	return ret;
}

bool j1Collision::Start() {
	return true;
}

bool j1Collision::PreUpdate() {

	RemoveDeletedColliders();

	return true;
}

// Called before render is available
bool j1Collision::Update(float dt) {
	
	CalculateCollisions();

	return true;
}

bool j1Collision::PostUpdate() {
	DebugDraw();

	return true;
}

void j1Collision::RemoveDeletedColliders() {

	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	
}

void j1Collision::CalculateCollisions() {
	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}
}

void j1Collision::DebugDraw()
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;

		case COLLIDER_WALL: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;

		case COLLIDER_TRANSPASSABLE_WALL: // purple
			App->render->DrawQuad(colliders[i]->rect, 128, 0, 128, alpha);
			break;

		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;

		case COLLIDER_EXPLOSION: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;

		case COLLIDER_END_LEVEL: //yellow
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
		}
	}

}

// Called before quitting
bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");


	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}


Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr) {

			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}	

		if (colliders[i]->type == type && colliders[i]->rect.y == rect.y && colliders[i]->rect.h == rect.h && (colliders[i]->rect.x + colliders[i]->rect.w == rect.x))
		{
			colliders[i]->rect.w += rect.w;
			break;
		}
	}

	return ret;
}

//simplified version of the above
Collider* j1Collision::AddCollider(Collider collider)
{
	Collider* ret = new Collider(collider.rect, collider.type, collider.callback);

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			colliders[i] = ret;
			break;
		}
	}

	return ret;
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return !(rect.x + rect.w < r.x ||
		rect.y + rect.h < r.y ||
		r.x + r.w < rect.x ||
		r.y + r.h < rect.y);
}