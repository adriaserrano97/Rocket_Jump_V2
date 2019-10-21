#include <math.h>
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Particles.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "p2Log.h"

#include "SDL/include/SDL_timer.h"

j1Particles::j1Particles()
{
	name.create("particles");

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;
}

j1Particles::~j1Particles()
{}

// Load assets
bool j1Particles::Start() {
	graphics = App->tex->Load(PATH(folder.GetString(), "particles.png"));
	graphics2 = App->tex->Load(PATH(folder.GetString(), "dust.png"));

	return true;
}
 
bool j1Particles::Awake(pugi::xml_node& node) {
	folder.create(node.child("folder").child_value());
	
	explosion.anim = explosion.anim.PushAnimation(node, "explosion");
	explosion.life = node.child("Animations").child("explosion").attribute("life").as_int();
	
	dust.anim = dust.anim.PushAnimation(node, "dust");
	dust.life = node.child("Animations").child("dust").attribute("life").as_int();

	return true;
}


// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");
	App->tex->UnLoad(graphics);
	App->tex->UnLoad(graphics2);

	graphics = nullptr;
	graphics2 = nullptr;

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr)
		{
			if (active[i]->collider != nullptr)
			{
				active[i]->collider->to_delete = true;
				active[i]->collider = nullptr;
			}

			active[i]->anim = Animation();
			delete active[i];
			active[i] = nullptr;
		}
	}

	
	
	return true;
}

// Update: draw background
bool j1Particles::Update(float dt)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = active[i];

		if (p == nullptr)
			continue;
		p->Update();

		if (p->collider != nullptr)
			p->collider->SetPos(p->position);
		

		App->render->Blit(graphics, p->position.x, p->position.y, &(p->anim.GetCurrentFrameBox()), p->flip);
		

		if (p != nullptr && p->life == 0)
		{
			if (p->collider != nullptr)
			{
				active[i]->collider->to_delete = true;
				active[i]->collider = nullptr;
			}
			

			delete active[i];
			active[i] = nullptr;
		}

		if (p->life > 0) {
			p->life--;
		}
	}
	return true;
}


void j1Particles::AddParticle(const Particle& particle, bool flip, int x, int y, int vx, int vy, COLLIDER_TYPE collider_type=COLLIDER_NONE, uint sound = 0, uint delay = 0)
//ADRI: the last 3 arguments are optional, thus have default parameter values. This way we can call this function whithout issue when our particle does not have those parameters associated
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] == nullptr)
		{
			Particle* p = new Particle(particle);
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
				p->collider = App->colliders->AddCollider(p->anim.GetCurrentFrameBox(), collider_type, this);

			active[i] = p;

			if (p->sfx != 0) {
				App->audio->PlayFx(p->sfx, 0);
			}

			break;
		}
	}
}

void j1Particles::OnCollision(Collider* c1, Collider* c2)
{
	
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
	fx(p.fx), life(p.life)
{}

bool Particle::Update()
{
	bool ret = true;

	position.x += speed.x;
	position.y += speed.y;

	return ret;
}