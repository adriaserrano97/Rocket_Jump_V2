#ifndef __j1Particles_H__
#define __j1Particles_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1Particles.h"

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
	Uint32 born = 0u;
	Uint32 life = 0u;
	bool flip;
	int number1;
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

	bool Update();
	bool CleanUp();

	//void AddParticle(const Particle& particle, bool flip, int x, int y, int vx, int vy, int ryu, COLLIDER_TYPE collider_type = COLLIDER_NONE, uint sound, uint delay = 0);
	void OnCollision(Collider* c1, Collider* c2);

private:

	SDL_Texture* graphics = nullptr;
	Particle* active[MAX_ACTIVE_PARTICLES];
	uint last_particle = 0;

public:
	//here goes the particles
};

#endif