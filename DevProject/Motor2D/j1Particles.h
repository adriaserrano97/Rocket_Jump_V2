#ifndef __j1Particles_H__
#define __j1Particles_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#define MAX_ACTIVE_PARTICLES 150

struct SDL_Texture;
struct Mix_Chunk;
struct Collider;
enum COLLIDER_TYPE;

struct Particle
{
	Collider* collider = nullptr;
	Animation anim;
	uint fx = 0u;
	iPoint position;
	iPoint speed;	
	Uint32 life = 0u;
	bool flip;
	uint sfx = 0u;
	Particle();
	Particle(const Particle& p);
	bool Update();
};

class j1Particles : public j1Module
{
public:
	j1Particles();
	~j1Particles();

	bool Awake(pugi::xml_node&);
	bool Start();

	bool Update(float dt);
	bool CleanUp();

	void AddParticle(const Particle& particle, bool flip, int x, int y, int vx, int vy, COLLIDER_TYPE collider_type, uint sound, uint delay);
	void OnCollision(Collider* c1, Collider* c2);

private:

	
	Particle* active[MAX_ACTIVE_PARTICLES];
	uint last_particle = 0;
	p2SString			folder;
	SDL_Texture* graphics = nullptr;

public:
	//here goes the particles
	Particle explosion;
	Animation explosion_animation;
};

#endif