#pragma once

#include "Module.h"
#include "PxPhysicsAPI.h"
#include "UtilitiesPhysX.h"

#include <wtypes.h>

using namespace physx;

// TODO: check if we still need this enum
enum class Space
{
	Global,
	Local
};

enum class ForceMode : uint
{
	FORCE,
	IMPULSE,
	ACCELERATION,
	VELOCITY_CHANGE,
	MAX
};

class ModulePhysX : public Module
{
	friend class ModuleObjects;
	friend class ComponentPhysics;
	friend class ComponentCollider;
	friend class ComponentBoxCollider;
	friend class ComponentSphereCollider;
	friend class ComponentCapsuleCollider;
	friend class ComponentConvexHullCollider;
	friend class ComponentBasePhysic;

public:

	ModulePhysX(bool start_enabled = true);
	~ModulePhysX();

public:
	// character controller
	PxController* CreateCharacterController(PxControllerDesc& desc);
	uint GetNbControllers() const;
	PxController* GetController(uint index) const;

	// material
	PxMaterial* CreateMaterial(float staticFriction, float dynamicFriction, float restitution) const;

private:

	// TODO DELETE ME, PVD purposes
	void CreateStack(const PxTransform& t, PxU32 size, PxReal halfExtent);
	/*void LoadConfig(JSONfilepack*& config);
	void SaveConfig(JSONfilepack*& config);*/

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

private:

	void DrawCollider(ComponentCollider* collider);
	void DrawWorld();

	PxRigidActor* CreateBody(const float4x4& transform, bool is_dynamic);
	void RemoveBody(PxRigidActor* body);

	PxShape* CreateShape(const PxGeometry& geometry);

	// Delay Libraries -----------------------------------------

	bool LoadPhysicsExplicitely();
	void UnloadPhysicsExplicitely();



private:

	HMODULE foundation_lib = nullptr;
	HMODULE common_lib = nullptr;
	HMODULE physx_lib = nullptr;
	HMODULE cooking_lib = nullptr;

	const char* foundation_lib_path = APP_BIN_DIR DLL_DIR "PhysXFoundation_" DLL_NAME_BITS ".dll";
	const char* common_lib_path = APP_BIN_DIR DLL_DIR "PhysXCommon_" DLL_NAME_BITS ".dll";
	const char* physx_lib_path = APP_BIN_DIR DLL_DIR "PhysX_" DLL_NAME_BITS ".dll";
	const char* gpu_lib_path = APP_BIN_DIR DLL_DIR "PhysXGpu_" DLL_NAME_BITS ".dll";
	const char* cooking_lib_path = APP_BIN_DIR DLL_DIR "PhysXCooking_" DLL_NAME_BITS ".dll";

private:
	
	PxDefaultAllocator			px_allocator;
	CustomErrorCallback			px_error_callback;

	PxFoundation*				px_foundation = nullptr;
	PxPhysics*					px_physics = nullptr;
	PxDefaultCpuDispatcher*		px_dispatcher = nullptr;
	PxScene*					px_scene = nullptr;
	PxMaterial*					px_default_material = nullptr;
	PxPvd*						px_pvd = nullptr;
	PxControllerManager*		controllers_manager = nullptr;

	PxReal stackZ = 10.0f;
	
public:
};

// TODO: move this block to another header (and defines up there) ---------------------------------------------------------------
// typedef the PhysX entry points
//typedef PxFoundation* (PxCreateFoundation_FUNC)(PxU32, PxAllocatorCallback&, PxErrorCallback&);
//typedef PxPhysics* (PxCreatePhysics_FUNC)(PxU32, PxFoundation&, const PxTolerancesScale& scale, bool, PxPvd*);
//typedef void (PxSetPhysXDelayLoadHook_FUNC)(const PxDelayLoadHook* hook);
//typedef void (PxSetPhysXCommonDelayLoadHook_FUNC)(const PxDelayLoadHook* hook);
//typedef void (PxSetPhysXGpuLoadHook_FUNC)(const PxGpuLoadHook* hook);
//typedef int (PxGetSuggestedCudaDeviceOrdinal_FUNC)(PxErrorCallback& errc);
//typedef PxCudaContextManager* (PxCreateCudaContextManager_FUNC)(PxFoundation& px_foundation, const PxCudaContextManagerDesc& desc, physx::PxProfilerCallback* profilerCallback);

//// set the function pointers to NULL
//PxCreateFoundation_FUNC* s_PxCreateFoundation_Func = NULL;
//PxCreatePhysics_FUNC* s_PxCreatePhysics_Func = NULL;
//PxSetPhysXDelayLoadHook_FUNC* s_PxSetPhysXDelayLoadHook_Func = NULL;
//PxSetPhysXCommonDelayLoadHook_FUNC* s_PxSetPhysXCommonDelayLoadHook_Func = NULL;
//PxSetPhysXGpuLoadHook_FUNC* s_PxSetPhysXGpuLoadHook_Func = NULL;
//PxGetSuggestedCudaDeviceOrdinal_FUNC* s_PxGetSuggestedCudaDeviceOrdinal_Func = NULL;
//PxCreateCudaContextManager_FUNC* s_PxCreateCudaContextManager_Func = NULL;