#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class ComponentMesh;
class ComponentRigidBody;
class ComponentVehicle;
class ComponentTransform;

class __declspec(dllexport) ComponentCollider : public Component
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ModulePhysics;
	friend class ReturnZ;
	friend class CompZ;
	friend class ComponentRigidBody;
	friend class ComponentVehicle;

public:

	ComponentCollider(GameObject* go);
	virtual ~ComponentCollider() {}

protected:

	float3 GetWorldCenter();

	// Virtual Functions ------------------------------

	virtual void Update();
	virtual void DrawCollider();
	virtual bool DrawInspector();

	virtual void Reset();
	virtual void Clone(Component* clone) {}
	virtual void SetComponent(Component* component) {}
	virtual void SaveComponent(JSONArraypack* to_save);
	virtual void LoadComponent(JSONArraypack* to_load);

	virtual void CreateShape() {};
	virtual void AdjustShapeToMesh() {}; 	// Adjust shape to scale and other factors

protected:

	float3 scaled_center = float3(0.f, 0.f, 0.f);

	float3 center; // Local position from GameObject
	Quat rotation; // Local rotation

	btCollisionShape* shape = nullptr;
	ComponentTransform* transform = nullptr;
	ComponentRigidBody* rigid_body = nullptr;
	int child_collider_index = 0;
	bool is_added = false;
};
