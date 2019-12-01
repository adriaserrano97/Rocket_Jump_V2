#include "Dust.h"
#include "j1EntityManager.h"
#include "j1Collision.h"
#include "j1Audio.h"

Dust::Dust(int x, int y) {

	position.x = x;
	position.y = y;

	animation = App->entityManager->dustAnimation;

	

	life = App->entityManager->dust_life;

	type = EntityTypes::DUST_PARTICLE;

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

	if (life * dt > 0) {
		life -= dt;
	}

	return true;
}