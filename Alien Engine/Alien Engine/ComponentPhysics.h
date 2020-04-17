#pragma once

#include "Component.h"
#include "PxShape.h"
#include <list>

using namespace physx;

class __declspec(dllexport) ComponentPhysics : public Component
{
	friend class GameObject;
	friend class SimulationEventCallback;

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
	void SwitchedRigidBody(ComponentRigidBody* rb);
	void AttachCollider(ComponentCollider* collider, bool to_update = false);    // To Update if is only a removement 
	void DettachColldier(ComponentCollider* collider, bool to_update = false);	 // to update the shape
	bool CheckRigidBody(ComponentRigidBody* rb);

	bool AddCollider(ComponentCollider* collider);     
	bool RemoveCollider(ComponentCollider* collider);	
 	bool FindCollider( ComponentCollider* collider);
	bool CheckCollider(ComponentCollider* collider);
	
	void GizmoManipulation();
	void UpdatePositioning();

	void UpdateBody();
	bool CheckChangeState();

	bool HasEnabledColliders();
	bool ShapeAttached(PxShape* shape);
	bool IsDynamic();
	bool IsKinematic();

protected:

	enum class PhysicState { STATIC = 0, DYNAMIC, CTRL_CHARACTER, CTRL_VEHICLE, DISABLED}
	state = PhysicState::DISABLED;

	bool gizmo_selected = false;

	ComponentTransform*			  transform = nullptr;
	std::list<ComponentCollider*> colliders;
	std::list<ComponentScript*>   scripts;
	ComponentRigidBody*			  rigid_body = nullptr;
	PxRigidActor*				  actor = nullptr;

};

