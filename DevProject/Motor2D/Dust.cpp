#include "Dust.h"
#include "j1EntityManager.h"
#include "j1Collision.h"
#include "j1Audio.h"

Dust::Dust(int x, int y) :
Particle(x, y, App->entityManager->dust_life, EntityTypes::DUST_PARTICLE, App->entityManager->dustAnimation)
{
}




bool Dust::Start() {

	texture = App->entityManager->spritesDust;
	started = true;

	return true;
}

bool Dust::Update(float dt) {

	if (life * dt <= 0)
	{
		to_delete = true;
	}

	else 
	{
		life -= dt;
	}
	

	return true;
}

