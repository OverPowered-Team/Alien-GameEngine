#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class ComponentMesh;
class ComponentRigidBody;
class ComponentVehicle;

class __declspec(dllexport) ComponentCollider : public Component
{
public:
	friend class ModuleObjects;
	friend class ModulePhysics;
	friend class GameObject;
	friend class Prefab;
	friend class ReturnZ;
	friend class CompZ;

	friend class ComponentRigidBody;
	friend class ComponentVehicle;

public:

	ComponentCollider(GameObject* go);

	virtual ~ComponentCollider() {}

	bool CleanUp();

	void Update();

	virtual bool Render();

	 // Common Functoins ------------------------------

	void SetBouncing(float& v);

	void SetFriction(float& v);

	void SetAngularFriction(float& v);

	void SetIsTrigger(bool is_trigger);

	bool GetIsTrigger();

	float3 GetWorldCenter();

private:

	bool  DrawInspector();

	void SaveComponent(JSONArraypack* to_save);

	void LoadComponent(JSONArraypack* to_load);

	//bool SearchRigidBody();

protected:

	// Virtual Functions ------------------------------

	virtual void Reset() {}

	virtual void SetComponent(Component* component) {}

	virtual void Clone(Component* clone) {}

	virtual void CreateShape(ComponentMesh* mesh) {}; // Create specific shape

	virtual void AdjustShape() {}; 	// Adjust shape to scale and other factors

	virtual void SaveCollider(JSONArraypack* config) {}

	virtual void LoadCollider(JSONArraypack* config) {}

	virtual void DrawInspectorCollider() {};

protected:

	float3 center = float3(0.f, 0.f, 0.f);
	float3 scaled_center = float3(0.f, 0.f, 0.f);

	bool is_trigger = false;
	float bouncing = 0.f;
	float friction = 0.f;
	float angular_friction = 0.f;

	btCollisionShape* shape = nullptr;
	btRigidBody* aux_body = nullptr;  // Used when GameObject has notrigid body in run time
	btDefaultMotionState* aux_motion_state = nullptr; 	// Used when GameObject has notrigid body in run time
	bool body_added = false; 	// Added to world

};
