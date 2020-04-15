#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentPhysics.h"
#include "ComponentCollider.h"
#include "ComponentRigidBody.h"

#include "ImGuizmos/ImGuizmo.h"
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

	state = PhysicState::STATIC;
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
	if (!actor) return;

	if (!Time::IsPlaying() || gizmo_selected)
	{
		PxTransform trans(F4X4_TO_PXTRANS(transform->GetGlobalMatrix()));
		actor->setGlobalPose(trans);
	}
	else
	{
		PxTransform trans = actor->getGlobalPose();
		transform->SetGlobalPosition(PXVEC3_TO_F3(trans.p));
		transform->SetGlobalRotation(PXQUAT_TO_QUAT(trans.q));
	}

	GizmoManipulation();

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
	case AlienEventType::COLLIDER_ENABLED: {
		ComponentCollider* object = (ComponentCollider*)e.object;
		AttachCollider(object);
		break; }
	case AlienEventType::COLLIDER_DISABLED: {
		ComponentCollider* object = (ComponentCollider*)e.object;
		DettachColldier(object);
		break; }
	case AlienEventType::RIGIDBODY_ADDED:
	case AlienEventType::RIGIDBODY_ENABLED: {
		ComponentRigidBody* object = (ComponentRigidBody*)e.object;
		AddRigidBody(object);
		break; }
	case AlienEventType::RIGIDBODY_DELETED:
	case AlienEventType::RIGIDBODY_DISABLED: {
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
		if (CheckChangeState()) UpdateBody();
		return true;
	}

	return true;
}

bool ComponentPhysics::RemoveRigidBody(ComponentRigidBody* rb)
{
	if (rb == rigid_body)
	{
		rigid_body = nullptr;
		if (CheckChangeState()) UpdateBody();
		return true;
	}

	return true;
}


void ComponentPhysics::AttachCollider(ComponentCollider* collider, bool only_update)
{
	bool do_attach = false;

	if (!only_update &&  CheckCollider(collider) && collider->IsEnabled())
	{
		if (CheckChangeState())
			UpdateBody();
		else
			do_attach = true;
	}

	if (actor && (do_attach || only_update ))
	{
		if (!ShapeAttached(collider->shape))	
			actor->attachShape(*collider->shape);
		if (IsDynamic())						
			actor->is<PxRigidDynamic>()->wakeUp();
	}
}

void ComponentPhysics::DettachColldier(ComponentCollider* collider, bool only_update)
{
	bool do_dettach = false;

	if (!only_update && CheckCollider(collider) && !collider->IsEnabled())
	{
		if (CheckChangeState())
			UpdateBody();
		else
			do_dettach = true;
	}

	if (actor && (do_dettach || only_update))
	{
		if (ShapeAttached(collider->shape))	     
			actor->detachShape(*collider->shape);
		if (IsDynamic())						 
			actor->is<PxRigidDynamic>()->wakeUp();
	}
}

// Add Collider from Phisic System 
bool ComponentPhysics::AddCollider(ComponentCollider* collider)
{
	if (CheckCollider(collider))
	{
		if (!FindCollider(collider))			// Add Collider if is not found
			colliders.push_back(collider);		
		if (CheckChangeState())					// Check if added collider change state
			UpdateBody();	
		else									// Else only attach 
		{
			AttachCollider(collider, true);
		}
		return true;
	}
	return false;
}

// Remove Collider from Phisic System 
bool ComponentPhysics::RemoveCollider(ComponentCollider* collider)
{
	if (FindCollider(collider))
	{
		colliders.remove(collider);
		if (CheckChangeState()) UpdateBody();
		else
		{
			DettachColldier(collider, true);
		}
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
	return (collider != nullptr && collider->game_object_attached == game_object_attached);
}

bool ComponentPhysics::CheckRigidBody(ComponentRigidBody* rb)
{
	return (rb != nullptr && rb->game_object_attached == game_object_attached && !rigid_body && rb != rigid_body);
}

void ComponentPhysics::UpdateBody()
{
	if (actor)  // Dettach and Delete Actor
	{
		PxU32 num_shapes = actor->getNbShapes();
		PxShape** shapes = new PxShape * [num_shapes]; // Buffer Shapes 
		actor->getShapes(shapes, num_shapes);

		for (PxU32 i = 0; i < num_shapes; ++i)
			actor->detachShape(*shapes[i]);

		delete[]shapes;

		App->physx->RemoveBody(actor);
		actor = nullptr;
	}

	if (state == PhysicState::STATIC || state == PhysicState::DYNAMIC || state == PhysicState::KINEMATIC)
	{
		actor = App->physx->CreateBody(transform->GetGlobalMatrix(), IsDynamic());

		for (ComponentCollider* collider : colliders)
			if (collider->enabled)
				actor->attachShape(*collider->shape);
	}
}

bool ComponentPhysics::CheckChangeState()
{
	if (rigid_body == nullptr && colliders.empty()) // Delete if not has physic components
	{
		Destroy();
		state = PhysicState::DISABLED;
		return false;
	}

	PhysicState new_state = PhysicState::DISABLED;

	if (rigid_body != nullptr && rigid_body->IsEnabled())
	{
		if (rigid_body->is_kinematic)
			new_state = PhysicState::KINEMATIC;
		else
			new_state = PhysicState::DYNAMIC;
	}
	else
	{
		if (HasEnabledColliders())
			new_state = PhysicState::STATIC;
		else
			new_state = PhysicState::DISABLED;
	}

	if (new_state != state)
	{
		state = new_state;
		return true;
	}

	return false;
}

void ComponentPhysics::GizmoManipulation()
{

	if (IsDynamic())
	{
		bool is_using_gizmo = ImGuizmo::IsUsing() && game_object_attached->IsSelected();
		PxRigidDynamic* dyn = actor->is<PxRigidDynamic>();

		if (gizmo_selected)
		{
			if (!is_using_gizmo)
			{
				dyn->wakeUp();
				gizmo_selected = false;
			}
			else
				dyn->putToSleep();
		}
		else
			if (is_using_gizmo)
				gizmo_selected = true;
	}
	else
		gizmo_selected = false;
}


bool ComponentPhysics::HasEnabledColliders()
{
	for (ComponentCollider* collider : colliders)
		if (collider->enabled)
			return true;

	return false;
}

bool ComponentPhysics::ShapeAttached(PxShape* shape)
{
	bool ret = false;
	PxU32 num_shapes = actor->getNbShapes();
	PxShape** shapes = new PxShape * [num_shapes]; // Buffer Shapes 
	actor->getShapes(shapes, num_shapes);

	for (PxU32 i = 0; i < num_shapes; i++)
		if (shapes[i] == shape)
		{
			ret = true;
			break;
		}

	delete[]shapes;
	return ret;
}