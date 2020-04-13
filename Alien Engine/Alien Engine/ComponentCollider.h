#pragma once

#include <map>
#include <list>
#include "ComponentBasePhysic.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Event.h"
#include <PxShape.h>

class GameObject;
class ComponentRigidBody;
class ComponentVehicle;
class ComponentTransform;
class ComponentPhysics;
class Alien;

using namespace physx;

class __declspec(dllexport) ComponentCollider : public ComponentBasePhysic
{
	friend class ModuleObjects;
	friend class ModulePhysX;
	friend class ComponentCharacterController;
	friend class ComponentRigidBody;
	friend class ComponentPhysics;

	friend class GameObject;
	friend class ReturnZ;
	friend class CompZ;
	friend class MyDispatcher;
public:

	ComponentCollider(GameObject* go);
	virtual ~ComponentCollider();


	// Colliders values
	void SetCenter(const float3& value);
	float3 GetCenter() { return center; }
	void SetRotation(const float3& value);
	float3 GetRotation() { return rotation; }
	void SetIsTrigger(bool is_trigger);
	bool GetIsTrigger() { return is_trigger; }
	void SetBouncing(const float bouncing);
	float GetBouncing() { return bouncing; }
	void SetFriction(const float v);
	float GetFriction() { return friction; }
	void SetAngularFriction(const float v);
	float GetAngularFriction() { return angular_friction; }

protected:

	void Update();
	void OnEnable();
	void OnDisable();

	bool DrawInspector();
	void HandleAlienEvent(const AlienEvent& e);

	virtual void DrawSpecificInspector() {}
	virtual void DrawScene();
	virtual void Reset();
	virtual void Clone(Component* clone) {}
	virtual void SetComponent(Component* component) {}
	virtual void SaveComponent(JSONArraypack* to_save);
	virtual void LoadComponent(JSONArraypack* to_load);

	virtual void CreateDefaultShape() {};

protected:

	float3 center = float3::zero();
	float3 rotation = float3::zero();
	bool is_trigger = false;

	float bouncing = 0.f;
	float friction = 0.f;
	float angular_friction = 0.f;

	std::list<ComponentScript*> alien_scripts;
	std::map<ComponentCollider*, bool> collisions;

	int  layer = 0;

	// ----------------------------------------------------------------

	PxShape* shape = nullptr;

};