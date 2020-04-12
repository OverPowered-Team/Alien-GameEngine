#include "Application.h"
#include "ComponentPhysic.h"
#include "ComponentCollider.h"
#include "ComponentRigidBody.h"
#include "GameObject.h"
#include "Event.h"

ComponentPhysic::ComponentPhysic(GameObject* go) : Component(go)
{

	ComponentRigidBody* found_rigid_body = game_object_attached->GetComponent<ComponentRigidBody>();
	AddRigidBody(found_rigid_body);

	std::vector<ComponentCollider*> found_colliders = game_object_attached->GetComponents<ComponentCollider>();
	for (int i = 0; i < found_colliders.size(); ++i)
		AddCollider(found_colliders[i]);
}

ComponentPhysic::~ComponentPhysic()
{
	rigid_body = nullptr;
}

void ComponentPhysic::Update()
{
}

void ComponentPhysic::HandleAlienEvent(const AlienEvent& e)
{
	switch (e.type) {
	case AlienEventType::COLLIDER_ADDED: {
		ComponentCollider* object = (ComponentCollider*)e.object;
		AddCollider(object);
		break; }
	case AlienEventType::COLLIDER_DELETED: {
		ComponentCollider* object = (ComponentCollider*)e.object;
		RemoveCollider(object);
		break; }
	case AlienEventType::RIGIDBODY_ADDED: {
		ComponentRigidBody* object = (ComponentRigidBody*)e.object;
		AddRigidBody(object);
		break; }
	case AlienEventType::RIGIDBODY_DELETED: {
		ComponentRigidBody* object = (ComponentRigidBody*)e.object;
		RemoveRigidBody(object);
		break; }
	}
}

bool ComponentPhysic::AddRigidBody(ComponentRigidBody* rb)
{
	if (CheckRigidBody(rb))
	{
		rigid_body = rb;
		return true;
	}

	return true;
}

bool ComponentPhysic::RemoveRigidBody(ComponentRigidBody* rb)
{
	if (rb == rigid_body)
	{
		rigid_body = nullptr;
		return true;
	}

	return true;
}

bool ComponentPhysic::AddCollider(ComponentCollider* collider)
{
	if (CheckCollider(collider))
	{
		colliders.push_back(collider);
		return true;
	}
	return false;
}

bool ComponentPhysic::RemoveCollider(ComponentCollider* collider)
{
	if (FindCollider(collider))
	{
		colliders.remove(collider);
		return true;
	}
	return false;
}

bool ComponentPhysic::FindCollider(ComponentCollider* collider)
{
	return (std::find(colliders.begin(), colliders.end(), collider) != colliders.end());
}

bool ComponentPhysic::CheckCollider(ComponentCollider* collider)
{
	return (collider != nullptr && collider->game_object_attached == game_object_attached && collider->IsEnabled() && !FindCollider(collider));
}

bool ComponentPhysic::CheckRigidBody(ComponentRigidBody* rb)
{
	return (rb != nullptr && rb->game_object_attached == game_object_attached && rb->IsEnabled() && !rigid_body);
}

void ComponentPhysic::UpdateBody()
{
	if (actor) App->physx->RemoveBody(actor);
	actor = App->physx->CreateBody(float4x4(), (rigid_body !=nullptr) );
}