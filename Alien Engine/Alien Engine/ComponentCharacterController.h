#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"

#include "PhysX/include/characterkinematic/PxController.h"

class GameObject;
class ModulePhysX;
class ComponentMesh;
class ComponentTransform;

class Alien;

class __declspec(dllexport) ComponentCharacterController : public Component//ComponentCollider
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ModulePhysX;
	friend class ReturnZ;
	friend class CompZ;
	friend class ComponentRigidBody;

public:

	ComponentCharacterController(GameObject* go);
	virtual ~ComponentCharacterController();

	//void Jump(float3 direction= float3::zero());
	//bool CanJump();
	//bool OnGround();

	//float GetJumpSpeed() { return jump_speed; }
	//void SetJumpSpeed(const float jump_speed);
	//float GetGravity() { return gravity; }
	//void SetGravity(const float fall_speed);

	//void ApplyImpulse(float3 direction = float3::zero());
	//void SetWalkDirection(float3 direction);

	//void SetRotation(const Quat rotation);
	//Quat GetRotation() const;

	//void SetPosition(const float3 pos);
	//float3 GetPosition() const;

	//void SetCharacterOffset(const float3 offset);
	//float GetCharacterHeight() { return character_height; }
	void SetCharacterHeight(const float height);
	//float GetCharacterRadius() { return character_radius; }
	void SetCharacterRadius(const float radius);

private:
	void SetDefaultConf();

protected:

	/*void RecreateCapusle();

	void Reset() {}*/
	void Clone(Component* clone) {}
	void Update();
	void DrawScene();
	bool DrawInspector();

	void HandleAlienEvent(const AlienEvent& e);
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

protected:
	ComponentTransform* transform = nullptr;
	/*ComponentCollider* collider = nullptr;
	btKinematicCharacterController* controller = nullptr;
	btPairCachingGhostObject* body = nullptr;
	btPairCachingGhostObject* detector = nullptr;
	btCapsuleShape* shape = nullptr;

	float3 character_offset = float3::zero();
	float character_height = 1.f;
	float character_radius = 0.5f;

	float jump_speed = 0.f;
	float gravity = 80.f;

	bool test = false;*/
	float gravity = 9.8f;
	bool isGrounded = false;

private:
	PxCapsuleControllerDesc desc;
	PxController* controller = nullptr;

	float dynamic_friction = 0.5f;
	float static_friction = 0.5f;
	float restitution = 0.6f;
};