#include "ModulePhysX.h"
#include "Optick/include/optick.h"


#ifdef _DEBUG
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\PhysX_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\PhysXCommon_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\PhysXFoundation_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\PhysXExtensions_static_32.lib") 
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\PhysXCooking_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\PhysXVehicle_static_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\PhysXCharacterKinematic_static_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\PhysXPvdSDK_static_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\SimulationController_static_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\LowLevelDynamics_static_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\SceneQuery_static_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\LowLevel_static_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\LowLevelAABB_static_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_debug\\PhysXTask_static_32.lib")
#endif

#ifdef NDEBUG
#pragma comment(lib, "PhysX\\lib_x86\\_release\\PhysX_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_release\\PhysXCommon_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_release\\PhysXFoundation_32.lib")
#pragma comment(lib, "PhysX\\lib_x86\\_release\\PhysXExtensions_static_32.lib")
#endif

#include "PhysX\include\PxPhysicsAPI.h"

ModulePhysX::ModulePhysX(bool start_enabled) : Module(start_enabled)
{
	gravity = float3(0.f, -9.8f, 0.f);
}

ModulePhysX::~ModulePhysX()
{
}



bool ModulePhysX::Init()
{
	OPTICK_EVENT();
	LOG_ENGINE("Init Nvidia PhysX 4.1.0");

	// Foundation -------------------------------------
	static physx::PxDefaultAllocator	px_allocator;
	static PhysicsErrorCallback			px_error_callback; 

	px_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, px_allocator, px_error_callback);
	px_foundation->setReportAllocationNames(false);
	// Physics -----------------------------------------

	bool record_memory_allocs = true; // Profiling activation

	px_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *px_foundation, physx::PxTolerancesScale(), record_memory_allocs);
	assert(px_physics == nullptr && "MODULE PHYSICS: PxCreatePhysics failed!");

	// Scene -------------------------------------------

	int num_threads = 1;
	px_dispatcher = physx::PxDefaultCpuDispatcherCreate(num_threads);

	physx::PxSceneDesc scene_description(px_physics->getTolerancesScale());
	scene_description.gravity = physx::PxVec3(gravity.x, gravity.y, gravity.z);
	scene_description.cpuDispatcher = px_dispatcher;
	//sceneDesc.filterShader = FilterShader;
	//sceneDesc.simulationEventCallback = simulationEventCallback;
	px_scene = px_physics->createScene(scene_description);
	assert(px_scene == nullptr && "MODULE PHYSICS: createScene failed!");

	// Default material
	px_default_material = px_physics->createMaterial(DEFAULT_STATIC_FRICTION, DEFAULT_DYNAMIC_FRICTION, DEFAULT_RESTIUTION);

	return false;
}

bool ModulePhysX::CleanUp()
{
	px_default_material->release();
	px_default_material = nullptr;

	px_scene->release();
	px_scene = nullptr;

	px_dispatcher->release();
	px_dispatcher = nullptr;

	px_physics->release();
	px_physics = nullptr;

	px_foundation->release();
	px_foundation = nullptr;

	return false;
}

bool ModulePhysX::Start()
{
	return false;
}

update_status ModulePhysX::PreUpdate(float dt)
{
	OPTICK_EVENT();

	return update_status();
}

update_status ModulePhysX::PostUpdate(float dt)
{
	return update_status();
}

bool ModulePhysX::CanCollide(int layer_0, int layer_1)
{
	return false;
}

void ModulePhysX::DrawCollider(ComponentCollider* collider)
{
}

void ModulePhysX::DrawConvexCollider(ComponentCollider* collider)
{
}

void ModulePhysX::LoadConfig(JSONfilepack*& config)
{
}

void ModulePhysX::SaveConfig(JSONfilepack*& config)
{
}

void ModulePhysX::SaveCollisionLayers()
{
}

void ModulePhysX::LoadCollisionLayers()
{
}

void ModulePhysX::CreateLayersTable()
{
}

void ModulePhysX::DeleteLayersTable()
{
}


