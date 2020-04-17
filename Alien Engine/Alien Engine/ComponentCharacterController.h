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
	// Auto resize from the bottom (moving position pivot automatly)
	void Resize(const float new_height);
	void SetSlopeLimit(const float slopeLimit);
	void SetStepOffset(const float stepOffset);
	void SetContactOffset(const float contactOffset);

	// TODO: make own copy of collision flag to not work with physx data outside.
	// Move by motion, this function doesn't apply any gravity,
	// (user implementation, or active force gravity in character controller component)
	PxControllerCollisionFlags Move(float3 motion);

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

public:
	PxControllerCollisionFlags collisionFlags;
	// The velocity returned is simply the difference in distance 
	// for the current timestep before and after a call to CharacterController.Move
	PxExtendedVec3 velocity;
	bool isGrounded = false;

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
	float gravity = 20.0f;//9.8f;
	

	// advanced options -------------
	// Forces move with zero vector when controller is idle, 
	// this forces messages from OnControllerColliderHit in any situation,
	// if off, messages from this callback only happen when a move is performed.
	bool force_move = true;
	// if off, any gravity needs to be implemented by the user
	// if on, forces gravity defined on gravity field always
	// that isGrounded is false
	bool force_gravity = true;

private:
	PxCapsuleControllerDesc desc;
	float min_distance;
	PxController* controller = nullptr;

	float dynamic_friction = 0.5f;
	float static_friction = 0.5f;
	float restitution = 0.6f;

	// internal extra functionality
	float3 moveDirection;
};