#include "Particle.h"
#include "j1Render.h"

Particle::Particle(int x, int y, float life, EntityTypes type, Animation animation) :
	Entity(iPoint(x, y), type, animation),
	life(life)
{
}


Particle::~Particle() {
	
	life = 0.0f;
}


void Particle::Draw(float dt)
{
		App->render->Blit(texture, position.x, position.y, &animation.GetCurrentFrameBox(dt), NULL, NULL, NULL, NULL, false);

}





