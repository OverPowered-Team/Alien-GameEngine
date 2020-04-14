#pragma once

#include "Component.h"
#include "PxPhysicsAPI.h"
#include <list>

class __declspec(dllexport) ComponentPhysics : public Component
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ModulePhysics;
	friend class ModulePhysX;
	friend class ComponentTransform;
	friend class ComponentCollider;
	friend class ComponentBoxCollider;
	friend class ComponentSphereCollider;
	friend class ComponentCapsuleCollider;
	friend class ComponentConvexHullCollider;
	friend class ComponentCharacterController;
	friend class ComponentRigidBody;

public:

	ComponentPhysics(GameObject* go);
	virtual ~ComponentPhysics();

private:

	void Update();
	void HandleAlienEvent(const AlienEvent& e);

	bool AddRigidBody(ComponentRigidBody* rb);
	bool RemoveRigidBody(ComponentRigidBody* rb);
	bool CheckRigidBody(ComponentRigidBody* rb);

	bool AddCollider(ComponentCollider* collider);
	bool RemoveCollider(ComponentCollider* collider);
	bool FindCollider( ComponentCollider* collider);
	bool CheckCollider(ComponentCollider* collider);

	void UpdateBody();
	void GizmoManipulation();

protected:

	bool is_dynamic = false;
	bool gizmo_selected = false;

	ComponentTransform*			  transform = nullptr;
	std::list<ComponentCollider*> colliders;
	ComponentRigidBody*			  rigid_body = nullptr;
	physx::PxRigidActor*		  actor = nullptr;

};

