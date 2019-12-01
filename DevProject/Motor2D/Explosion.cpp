#include "Explosion.h"
#include "j1EntityManager.h"
#include "j1Collision.h"
#include "j1Audio.h"

Explosion::Explosion(int x, int y) {

	position.x = x;
	position.y = y;

	animation = &App->entityManager->explosionAnimation;

	collider = App->colliders->AddCollider({ x, y, animation->GetRect().w, animation->GetRect().h }, COLLIDER_EXPLOSION, (j1Module*)App->entityManager);

	life = App->entityManager->explosion_life;

	type = EntityTypes::EXPLOSION_PARTICLE;

}

bool Explosion::Start() {

	texture = App->entityManager->spritesDust;
	started = true;

	return true;
}




Collider* Explosion::GetCollider() const
{
	return collider;
}

bool Explosion::Update(float dt) {

	if (life * dt <= 0)
	{
		collider->to_delete = true;
		collider = nullptr;

		to_delete = true;
	}

	if (life * dt > 0) {
		life -= dt;
	}

	return true;
}