#pragma once

#include "ComponentBasePhysic.h"
#include "ModulePhysics.h"
#include "MathGeoLib/include/Math/MathAll.h"

class GameObject;
class ComponentCollider;
class ComponentCapsuleCollider;

using namespace physx;

class __declspec(dllexport) ComponentRigidBody : public ComponentBasePhysic
{
	friend class GameObject;
	friend class ModulePhysics;
	friend class ComponentPhysics;
	friend class ComponentCollider;
	friend class ComponentBoxCollider;
	friend class ComponentSphereCollider;
	friend class ComponentCapsuleCollider;
	friend class ComponentConvexHullCollider;
	friend class ResourcePrefab;

public:

	ComponentRigidBody(GameObject* go);
	~ComponentRigidBody();

	// Forces Functions

	void AddForce(const float3 force, ForceMode mode = ForceMode::IMPULSE, Space space = Space::Global);
	void AddTorque(const float3 force, ForceMode mode = ForceMode::IMPULSE, Space space = Space::Global);

	// Rigid Body Values 
	void SetPosition(const float3 pos);
	void SetRotation(const Quat rotation);
	float3 GetPosition();
	Quat GetRotation();
	void SetTransform(const float3 position, const Quat rotation);

	void SetIsKinematic(const bool value);
	bool GetIsKinematic() { return is_kinematic; }
	void SetMass(const float mass);
	float GetMass() { return mass; }
	void SetDrag(const float drag);
	float GetDrag() { return drag; }
	void SetAngularDrag(const float angular_drag);
	float GetAngularDrag() { return angular_drag; }

	void SetFreezePosition(bool x, bool y, bool z);
	void GetFreezePosition(bool values[3]);
	void SetFreezeRotation(bool x, bool y, bool z);
	void GetFreezeRotation(bool values[3]);
				
	float3 GetVelocity();
	void SetVelocity(const float3 velocity);
	float3 GetAngularVelocity();
	void SetAngularVelocity(const float3 velocity);

private:

	void OnEnable();
	void OnDisable();
	void Update();
	bool DrawInspector();

	void Reset(){}
	void Clone(Component* clone) {}
	void SaveComponent(JSONArraypack* config);
	void LoadComponent(JSONArraypack* config);

	bool CanUseRigidBody();

private:

	float mass = 0.0f;
	float drag = 0.f;
	float angular_drag = 0.f;
	bool use_gravity = true;
	bool is_kinematic = false;
	bool freeze_position[3] = { false, false, false };
	bool freeze_rotation[3] = { false, false, false };
	
	PxRigidDynamic* body = nullptr;
};

