#pragma once

#include "Module.h"
#include <list>
#include <vector>



#include "PhysxAbstraction.h"
#include "PhysX\include\PxPhysicsAPI.h"
#include "PhysX\include\extensions\PxDefaultAllocator.h"
#include "PhysX\include\extensions\PxDefaultCpuDispatcher.h"
 
class ComponentCollider;

class ModulePhysX : public Module
{
	friend class Application;
	friend class ComponentCharacterController;
	friend class ModuleObjects;
	friend class ComponentCollider;
	friend class ComponentRigidBody;
	friend class ComponentPointConstraint;
	friend class ComponentConvexHullCollider;
	friend class PanelPhysics;

public:

	ModulePhysX(bool start_enabled = true);
	~ModulePhysX();

	bool CollisionLayerExist(std::string layer);
	void AddCollisionLayer(std::string layer);
	void RemoveCollisionLayer(std::string layer);

	void SetGravity(const float3 gravity);
	const float3 GetGravity();

	std::vector<ComponentCollider*> SphereCast(float3 position, float radius);
	std::vector<ComponentCollider*> BoxCast(float3 size, float3 position, Quat rotation);
	std::vector<ComponentCollider*> RayCastAll(math::Ray ray);
	ComponentCollider*				RayCastClosest(math::Ray ray);

private:

	bool Init();
	bool CleanUp();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);

	static bool CanCollide(int layer_0, int layer_1);

	void DrawCollider(ComponentCollider* collider);
	void DrawConvexCollider(ComponentCollider* collider);

	void LoadConfig(JSONfilepack*& config);
	void SaveConfig(JSONfilepack*& config);
	void SaveCollisionLayers();
	void LoadCollisionLayers();

	void CreateLayersTable();
	void DeleteLayersTable();

private:

	// General ----------------------

	float3 gravity;
	physx::PxMaterial* px_default_material = nullptr;

	// Debug------------------------

	bool debug_physics = false;

	// Collision Layers ------------

	std::vector<std::string> layers;
	bool** layers_table = nullptr;

	// Simulation ---------------------

	physx::PxFoundation* px_foundation = nullptr;
	physx::PxDefaultCpuDispatcher* px_dispatcher = nullptr;
	physx::PxPhysics* px_physics = nullptr;
	physx::PxScene* px_scene = nullptr;
	float gAccumulator = 0.0f;
};



