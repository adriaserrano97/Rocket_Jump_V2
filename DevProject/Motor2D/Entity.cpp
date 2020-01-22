#include "Entity.h"

Entity::Entity() {}

Entity::Entity(iPoint position, EntityTypes type, Animation animation) :
	position(position),
	type(type),
	animation(animation),
	texture(nullptr),
	to_delete(false),
	started(false)
{
}

Entity::~Entity() {

	position = {0, 0};
	type = EntityTypes::FAIL;
	texture = nullptr;
	animation = Animation();
}

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
	started = true;
	return true;
}

void Entity::Draw(float dt) {}

void Entity::OnCollision(Collider* collider) {}

Collider* Entity::GetCollider() const {
	return nullptr;
}

