#ifndef __j1Particles_H__
#define __j1Particles_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#define MAX_ACTIVE_Particles 150

struct SDL_Texture;
struct Mix_Chunk;
struct Collider;
enum COLLIDER_TYPE;

struct Particles
{
	Collider* collider = nullptr;
	Animation anim;
	uint fx = 0u;
	iPoint position;
	iPoint speed;	
	float life = 0.0f;
	bool flip;
	uint sfx = 0u;
	Particles();
	Particles(const Particles& p);
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

	void AddParticles(const Particles& Particles, bool flip, int x, int y, int vx, int vy, COLLIDER_TYPE collider_type, uint sound, uint delay);
	void OnCollision(Collider* c1, Collider* c2);

private:

	
	Particles* active[MAX_ACTIVE_Particles];
	uint last_Particles = 0;
	p2SString			folder;
	SDL_Texture* graphics = nullptr;

public:
	//list of Particless
	Particles explosion;
	Particles dust;

	//list of animations of such Particless 
	Animation dust_animation;
};

#endif