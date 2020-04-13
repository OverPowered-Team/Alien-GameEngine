#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentPhysics.h"
#include "ComponentCollider.h"
#include "ComponentRigidBody.h"
#include "GameObject.h"
#include "Event.h"
#include "Time.h"

ComponentPhysics::ComponentPhysics(GameObject* go) : Component(go)
{
	serialize = false;  // Not save & load 
	transform = go->GetComponent<ComponentTransform>();

	ComponentRigidBody* found_rigid_body = go->GetComponent<ComponentRigidBody>();
	AddRigidBody(found_rigid_body);

	std::vector<ComponentCollider*> found_colliders = go->GetComponents<ComponentCollider>();
	for (ComponentCollider* collider : found_colliders)
		AddCollider(collider);

	UpdateBody();
}

ComponentPhysics::~ComponentPhysics()
{
	App->physx->RemoveBody(actor);
	actor = nullptr;
	rigid_body = nullptr;
}

void ComponentPhysics::Update()
{
	if (Time::IsPlaying())
	{
		PxTransform trans = actor->getGlobalPose();
		transform->SetGlobalPosition(PXVEC3_TO_F3(trans.p));
		transform->SetGlobalRotation(PXQUAT_TO_QUAT(trans.q));
	}
	else
	{
		PxTransform trans(F4X4_TO_PXTRANS(transform->GetGlobalMatrix()));
		actor->setGlobalPose(trans);
	}
	PxTransform trans;
}

void ComponentPhysics::HandleAlienEvent(const AlienEvent& e)
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

bool ComponentPhysics::AddRigidBody(ComponentRigidBody* rb)
{
	if (CheckRigidBody(rb))
	{
		rigid_body = rb;
		UpdateBody();
		return true;
	}

	return true;
}

bool ComponentPhysics::RemoveRigidBody(ComponentRigidBody* rb)
{
	if (rb == rigid_body)
	{
		rigid_body = nullptr;
		UpdateBody();
		return true;
	}

	return true;
}

bool ComponentPhysics::AddCollider(ComponentCollider* collider)
{
	if (CheckCollider(collider))
	{
		colliders.push_back(collider);
		actor->attachShape(*collider->shape);
		return true;
	}
	return false;
}

bool ComponentPhysics::RemoveCollider(ComponentCollider* collider)
{
	if (FindCollider(collider))
	{
		colliders.remove(collider);
		actor->detachShape(*collider->shape);
		return true;
	}
	return false;
}

bool ComponentPhysics::FindCollider(ComponentCollider* collider)
{
	return (std::find(colliders.begin(), colliders.end(), collider) != colliders.end());
}

bool ComponentPhysics::CheckCollider(ComponentCollider* collider)
{
	return (collider != nullptr && collider->game_object_attached == game_object_attached && collider->IsEnabled() && !FindCollider(collider));
}

bool ComponentPhysics::CheckRigidBody(ComponentRigidBody* rb)
{
	return (rb != nullptr && rb->game_object_attached == game_object_attached && rb->IsEnabled() && !rigid_body && rb != rigid_body);
}

void ComponentPhysics::UpdateBody()
{
	if (actor) {

		for (ComponentCollider* collider : colliders)
			actor->detachShape(*collider->shape);

		App->physx->RemoveBody(actor);
	}

	actor = App->physx->CreateBody(transform->GetGlobalMatrix(), (rigid_body != nullptr));

	for (ComponentCollider* collider : colliders)
		actor->attachShape(*collider->shape);
}