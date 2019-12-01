#include "Particle.h"
#include "j1Render.h"

Particle::~Particle() {}

void Particle::Draw(float dt)
{

	if (animation != nullptr) {

		App->render->Blit(texture, position.x, position.y, &animation->GetCurrentFrameBox(dt), NULL, NULL, NULL, NULL, false);


	}
}





