#include "Entity.h"

bool Entity::Start()
{
	return true;
}

// Called each loop iteration
bool Entity::PreUpdate()
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