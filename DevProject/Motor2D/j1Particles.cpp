#include <math.h>
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Particles.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "p2Log.h"

#include "SDL/include/SDL_timer.h"

j1Particles::j1Particles()
{
	name.create("Particles");

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;
}

j1Particles::~j1Particles()
{}

// Load assets
bool j1Particles::Start()
{

	//graphics = App->tex->Load("assets/images/sprites/sfx/sfx.png");
	return true;
}

bool j1Particles::Awake(pugi::xml_node& node){

	return true;
}


// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");
	App->tex->UnLoad(graphics);
	graphics = nullptr;

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr)
		{
			delete active[i];
			active[i] = nullptr;
		}
	}

	return true;
}

// Update: draw background
bool j1Particles::Update()
{
	/*for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = active[i];

		if (p == nullptr)
			continue;
		p->Update();
		p->collider->SetPos(p->position);
		if (SDL_GetTicks() >= p->born)
		{
			App->render->Blit(graphics, p->position.x, p->position.y, &(p->anim.GetCurrentFrameBox()), p->flip);
		}
		if (active[i] != nullptr && p->life == 0)
		{
			active[i]->collider->to_delete = true;

			delete active[i];
			active[i] = nullptr;
		}
		if (p->life > 0) {
			p->life--;
		}
	}*/
	return true;
}

/*
void j1Particles::AddParticle(const Particle& particle, bool flip, int x, int y, int vx, int vy, int ryu, COLLIDER_TYPE collider_type, uint sound, uint delay)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] == nullptr)
		{
			Particle* p = new Particle(particle);
			p->born = SDL_GetTicks() + delay;
			p->position.x = x;
			p->position.y = y;
			p->sfx = sound;

			if (flip == false) {
				p->speed = { vx, vy };
				p->flip = false;
			}

			if (flip == true) {
				p->speed = { -vx, vy };
				p->flip = true;
			}

			if (collider_type != COLLIDER_NONE)
				//p->collider = App->collider->AddCollider(p->anim.GetCurrentFrameBox(), collider_type, this);

			active[i] = p;

			if (p->sfx != 0) {
				App->audio->PlayFx(p->sfx, 0);
			}

			break;
		}
	}
}*/

void j1Particles::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		// Always destroy particles that collide
		if (active[i] != nullptr && active[i]->collider == c1)
		{
			active[i]->collider->to_delete = true;

			delete active[i];
			active[i] = nullptr;
			break;
		}
	}
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Particle::Particle()
{
	position.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) :
	anim(p.anim), position(p.position), speed(p.speed),
	fx(p.fx), born(p.born), life(p.life)
{}

bool Particle::Update()
{
	bool ret = true;

	position.x += speed.x;
	position.y += speed.y;

	return ret;
}