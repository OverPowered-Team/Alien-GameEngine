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
class btKinematicCharacterController;
class btPairCachingGhostObject;
class Alien;

class __declspec(dllexport) ComponentCharacterController : public Component
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ModulePhysics;
	friend class ReturnZ;
	friend class CompZ;
	friend class ComponentRigidBody;

public:

	ComponentCharacterController(GameObject* go);
	virtual ~ComponentCharacterController();

	void Jump(float3 direction= float3::zero());
	bool CanJump();
	void SetJumpSpeed(const float jump_speed);
	void SetMaxJumpHeight(const float height);
	void SetFallSpeed(const float fall_speed);

	void SetWalkDirection(float3 direction);
	void SetRotation(const Quat rotation);

protected:

	void Update();
	void DrawScene();
	bool DrawInspector();

	virtual void Reset();
	virtual void Clone(Component* clone) {}
	virtual void SetComponent(Component* component) {}
	virtual void SaveComponent(JSONArraypack* to_save);
	virtual void LoadComponent(JSONArraypack* to_load);

protected:
	ComponentTransform* transform = nullptr;
	btKinematicCharacterController* controller = nullptr;
	btPairCachingGhostObject* body = nullptr;
	btCapsuleShape* shape = nullptr;

	float max_jump_height = 0.f;
	float jump_speed = 0.f;
	float fall_speed = 0.f;


	bool test = false;
};