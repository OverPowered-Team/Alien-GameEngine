#pragma once

#include "Component.h"
#include "PxPhysicsAPI.h"
#include <list>

class __declspec(dllexport) ComponentPhysic : public Component
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ModulePhysics;
	friend class ModulePhysX;
	friend class ComponentCharacterController;
	friend class ComponentRigidBody;

public:

	ComponentPhysic(GameObject* go);
	virtual ~ComponentPhysic();

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

protected:

	bool is_dynamic = false;

	std::list<ComponentCollider*> colliders;
	ComponentRigidBody*			  rigid_body = nullptr;
	physx::PxRigidActor*		  actor = nullptr;

};

