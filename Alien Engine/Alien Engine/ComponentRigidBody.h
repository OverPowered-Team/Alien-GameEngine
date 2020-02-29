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
	friend class GameObject;
	friend class ModulePhysics;
	friend class ComponentCollider;
	friend class ComponentBoxCollider;
	friend class ComponentSphereCollider;
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

private:

	void Update();
	bool DrawInspector();
	void Reset();
	void Clone(Component* clone);
	void SaveComponent(JSONArraypack* config);
	void LoadComponent(JSONArraypack* config);

	void AddCollider(ComponentCollider* collider);
	void UpdateCollider();
	void RemoveCollider();
	void UpdateBodyInertia();

	void SetBodyTranform(const float3& pos, const Quat& rot);

private:
	ComponentTransform* transform = nullptr;
	ComponentCollider* collider = nullptr;

	float3 velocity;
	btVector3 inertia;

	float3 force_to_apply[(uint)ForceMode::MAX];
	float3 torque_to_apply[(uint)ForceMode::MAX];

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
	btBoxShape* aux_shape = nullptr;
};

#endif // !_C_RIGID_BODY_H__

