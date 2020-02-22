#ifndef _C_RIGID_BODY_H__
#define  _C_RIGID_BODY_H__

#include "Component.h"
#include "ModulePhysics.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ComponentCollider;
class ComponentCapsuleCollider;
//class C_JointP2P;
//class C_Vehicle;

class __declspec(dllexport) ComponentRigidBody : public Component
{
	friend class ModulePhysics;
	friend class ComponentCollider;
	//friend class C_JointP2P;
	//friend class C_Vehicle;

public:

	ComponentRigidBody(GameObject* go);
	~ComponentRigidBody();

	// Forces Functions

	void AddForce(const float3 force, ForceMode mode = ForceMode::IMPULSE, Space space = Space::Global);
	void AddTorque(const float3 force, ForceMode mode = ForceMode::IMPULSE, Space space = Space::Global);

	// Rigid Body Values 

	void SetMass(const float mass);
	float GetMass() { return mass; }
	void SetDrag(const float drag);
	float GetDrag() { return drag; }
	void SetAngularDrag(const float angular_drag);

	float3 GetVelocity();
	void SetVelocity(const float3 velocity);
	float3 GetAngularVelocity();
	void SetAngularVelocity(const float3 velocity);

	// Colliders values

	void SetIsTrigger(bool is_trigger);
	bool GetIsTrigger() { return is_trigger; }
	void SetBouncing(const float bouncing);
	float GetBouncing() { return bouncing; }
	void SetFriction(const float v);
	float GetFriction() { return friction; }
	void SetAngularFriction(const float v);
	float GetAngularFriction() { return angular_friction; }

private:

	void Update();
	void Render();
	bool DrawInspector();
	void Reset();
	void Clone(Component* clone);
	void SaveComponent(JSONArraypack* config);
	void LoadComponent(JSONArraypack* config);

	void CreateBody();
	void SetBodyTranform(const float3& pos, const Quat& rot);

private:

	float3 velocity;
	btVector3 inertia;

	float3 force_to_apply[(uint)ForceMode::MAX];
	float3 torque_to_apply[(uint)ForceMode::MAX];

	bool is_trigger = false;
	float bouncing = 0.f;
	float friction = 0.f;
	float angular_friction = 0.f;

	float mass = 0.0f;
	float drag = 0.f;
	float angular_drag = 0.f;

	bool use_gravity = true;
	bool is_kinematic = false;
	bool freeze_position[3] = { false, false, false };
	bool freeze_rotation[3] = { false, false, false };

	// Body used in physics simulation
	btRigidBody* body = nullptr;
	// Used when GameObejct has not a collider
	btCompoundShape* compound_shape = nullptr;
	// Related components
	ComponentTransform* transform = nullptr;
	// MotionState 
	btMotionState* motion_state = nullptr;
};

#endif // !_C_RIGID_BODY_H__


