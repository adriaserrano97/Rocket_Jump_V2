#include "Particle.h"
#include "j1Render.h"

Particle::~Particle() {
	
	texture = nullptr;

	animation = Animation();
}

void Particle::Draw(float dt)
{

		App->render->Blit(texture, position.x, position.y, &animation.GetCurrentFrameBox(dt), NULL, NULL, NULL, NULL, false);

}





