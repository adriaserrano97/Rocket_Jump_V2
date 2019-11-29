#include "Entity.h"

bool Entity::Start()
{
	return true;
}

// Called each loop iteration
bool Entity::HandleInput()
{
	return true;
}

// Called each loop iteration
bool Entity::Update(float dt)
{
	return true;
}

// Called each loop iteration
bool Entity::PostUpdate()
{
	return true;
}

void Entity::Draw(float dt) {}

void Entity::OnCollision(Collider* collider) {

}

Collider* Entity::GetCollider() const {
	return nullptr;
}

