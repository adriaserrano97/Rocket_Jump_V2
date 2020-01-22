#include "Coin.h"
#include "j1EntityManager.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Gui.h"


Coin::Coin(int x, int y) :
	Entity(iPoint(x, y), EntityTypes::COIN, App->entityManager->coinanimation),
	collider(App->colliders->AddCollider({ x, y, animation.GetRect().w, animation.GetRect().h }, COLLIDER_COIN, (j1Module*)App->entityManager))
{
}

Coin::~Coin() {

	if (collider != nullptr) {

		collider->to_delete = true;
		collider = nullptr;
	}
}

void Coin::Destroy() {

	to_delete = true;
}

bool Coin::Start()
{
	texture = App->entityManager->powerups;
	started = true;

	return true;
}

bool Coin::Update(float dt)
{
	//whatevs
	return false;
}

void Coin::Draw(float dt)
{
	if (collider != nullptr) {
		collider->SetPos(position.x, position.y);
	}

	App->render->Blit(texture, position.x, position.y, &animation.GetCurrentFrameBox(dt), NULL, NULL, NULL, NULL);
}

void Coin::OnCollision(Collider * collider)
{
	if (collider->type == COLLIDER_PLAYER)
	{
		App->audio->PlayFx(App->audio->coin);
		App->gui->PlayerCoinsCounter++;
		App->gui->PlayerScoreCounter += 10;
		App->gui->UpdateLifesNCoins();
		Destroy();
	}
}

Collider * Coin::GetCollider() const
{
	return collider;
}
