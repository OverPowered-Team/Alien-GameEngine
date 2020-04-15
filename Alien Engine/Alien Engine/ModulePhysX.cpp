#include "Globals.h"
#include "Application.h"
#include "ModulePhysX.h"
#include "ModuleRenderer3D.h"

#include "Component.h"
#include "ComponentPhysics.h"
#include "ComponentCollider.h"
#include "ComponentRigidBody.h"
#include "PxPhysicsAPI.h"
#include "Time.h"
#include "UtilitiesPhysX.h"
#include "Optick/include/optick.h"

ModulePhysX::ModulePhysX(bool start_enabled) : Module(start_enabled)
{

}

ModulePhysX::~ModulePhysX()
{
}

void ModulePhysX::CreateStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = px_physics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *px_default_material);
	for (PxU32 i = 0; i < size; i++)
	{
		for (PxU32 j = 0; j < size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			PxRigidDynamic* body = px_physics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			px_scene->addActor(*body);
		}
	}
	shape->release();
}



bool ModulePhysX::Init()
{
	OPTICK_EVENT();
	LOG_ENGINE("Creating 3D PhysX world");
	bool ret = true;

	// load the explictely named dlls
	const bool isLoaded = LoadPhysicsExplicitely(); // load debug or release dlls
	if (!isLoaded)
		return false;

	px_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, px_allocator, px_error_callback);

	// set PhysX and PhysXCommon delay load hook, this must be done before the create physics is called, before ----
	// the PhysXFoundation, PhysXCommon delay load happens.
	CustomDelayLoadHook delayLoadHook;
	PxSetPhysXDelayLoadHook(&delayLoadHook);
	PxSetPhysXCommonDelayLoadHook(&delayLoadHook);
	PxSetPhysXCookingDelayLoadHook(&delayLoadHook);

	// set PhysXGpu load hook
	CustomGpuLoadHook gpuLoadHook;
	PxSetPhysXGpuLoadHook(&gpuLoadHook);
	// --------------------------------------------------------------------------------------------------------------

	px_pvd = PxCreatePvd(*px_foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	px_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	px_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *px_foundation, PxTolerancesScale(), true, px_pvd);

	PxSceneDesc sceneDesc(px_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	px_dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = px_dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	px_scene = px_physics->createScene(sceneDesc);
	px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	PxPvdSceneClient* pvdClient = px_scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	px_default_material = px_physics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*px_physics, PxPlane(0, 1, 0, 0), *px_default_material);

	px_scene->addActor(*groundPlane);

	//for (PxU32 i = 0; i < 5; i++)
	//	CreateStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);

	///*if (!interactive)
	//	createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));*/

	return ret;
}

bool ModulePhysX::Start()
{
	LOG_ENGINE("Creating Physics environment");


	return true;
}

// ---------------------------------------------------------
update_status ModulePhysX::PreUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

update_status ModulePhysX::PostUpdate(float dt)
{
	static bool first_frame_playing = true;
	OPTICK_EVENT();

	float fixed_dt = 1.0f / 60.0f;
	float game_dt = Time::GetDT();

	if (Time::IsPlaying())
	{
		px_scene->simulate(fixed_dt); // TODO, fixed time step / substeps
		px_scene->fetchResults(true);
	}

	return UPDATE_CONTINUE;
}

bool ModulePhysX::CleanUp()
{

	PX_RELEASE(px_scene);
	PX_RELEASE(px_dispatcher);
	PX_RELEASE(px_physics);
	if (px_pvd)
	{
		PxPvdTransport* transport = px_pvd->getTransport();
		px_pvd->release();	px_pvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(px_foundation);

	UnloadPhysicsExplicitely();

	return true;
}



bool ModulePhysX::LoadPhysicsExplicitely()
{
	// load the dlls

	foundation_lib = LoadLibraryA(foundation_lib_path);
	if (!foundation_lib)
		return false;

	common_lib = LoadLibraryA(common_lib_path);
	if (!common_lib)
	{
		FreeLibrary(foundation_lib);
		return false;
	}

	physx_lib = LoadLibraryA(physx_lib_path);
	if (!physx_lib)
	{
		FreeLibrary(foundation_lib);
		FreeLibrary(common_lib);
		return false;
	}

	cooking_lib = LoadLibraryA(cooking_lib_path);
	if (!cooking_lib)
	{
		FreeLibrary(foundation_lib);
		FreeLibrary(common_lib);
		FreeLibrary(physx_lib);
		return false;
	}

	//// get the function pointers
	//s_PxCreateFoundation_Func = (PxCreateFoundation_FUNC*)GetProcAddress(foundation_lib, "PxCreateFoundation");
	//s_PxCreatePhysics_Func = (PxCreatePhysics_FUNC*)GetProcAddress(physx_lib, "PxCreateBasePhysics");
	//s_PxSetPhysXDelayLoadHook_Func = (PxSetPhysXDelayLoadHook_FUNC*)GetProcAddress(physx_lib, "PxSetPhysXDelayLoadHook");
	//s_PxSetPhysXCommonDelayLoadHook_Func = (PxSetPhysXCommonDelayLoadHook_FUNC*)GetProcAddress(common_lib, "PxSetPhysXCommonDelayLoadHook");

	//s_PxSetPhysXGpuLoadHook_Func = (PxSetPhysXGpuLoadHook_FUNC*)GetProcAddress(physx_lib, "PxSetPhysXGpuLoadHook");
	//s_PxGetSuggestedCudaDeviceOrdinal_Func = (PxGetSuggestedCudaDeviceOrdinal_FUNC*)GetProcAddress(physx_lib, "PxGetSuggestedCudaDeviceOrdinal");
	//s_PxCreateCudaContextManager_Func = (PxCreateCudaContextManager_FUNC*)GetProcAddress(physx_lib, "PxCreateCudaContextManager");

	//// check if we have all required function pointers
	//if (s_PxCreateFoundation_Func == NULL || s_PxCreatePhysics_Func == NULL || s_PxSetPhysXDelayLoadHook_Func == NULL || s_PxSetPhysXCommonDelayLoadHook_Func == NULL)
	//	return false;

	//if (s_PxSetPhysXGpuLoadHook_Func == NULL || s_PxGetSuggestedCudaDeviceOrdinal_Func == NULL || s_PxCreateCudaContextManager_Func == NULL)
	//	return false;
	return true;
}

void ModulePhysX::UnloadPhysicsExplicitely()
{
	FreeLibrary(physx_lib);
	FreeLibrary(common_lib);
	FreeLibrary(foundation_lib);
}

void ModulePhysX::DrawCollider(ComponentCollider* collider)
{
	float4x4 trans(PXTRANS_TO_F4X4(collider->physics->actor->getGlobalPose()* collider->shape->getLocalPose()));
	float3 color = float3(0.f, 1.f, 0.f);

	switch (collider->type)
	{
	case ComponentType::BOX_COLLIDER: {
		PxBoxGeometry geo; 
		collider->shape->getBoxGeometry(geo);
		App->renderer3D->DebugDrawBox(trans, PXVEC3_TO_F3(geo.halfExtents), color);
		break; }
	case ComponentType::SPHERE_COLLIDER: {
		PxSphereGeometry geo;
		collider->shape->getSphereGeometry(geo);
		App->renderer3D->DebugDrawSphere(trans, geo.radius, color);
		break; }
	case ComponentType::CAPSULE_COLLIDER: {
		PxCapsuleGeometry geo;
		collider->shape->getCapsuleGeometry(geo);
		App->renderer3D->DebugDrawCapsule(trans, geo.radius, geo.halfHeight, color);
		break; }
	//case ComponentType::CONVEX_HULL_COLLIDER: {
	//	App->renderer3D->DebugDrawBox();
	//	break; }

	default:
		break;
	}
}

void ModulePhysX::DrawWorld()
{
	ModuleRenderer3D::BeginDebugDraw(float4(0.f, 1.f, 0.f, 1.f));

	const PxRenderBuffer& rb = px_scene->getRenderBuffer();
	PxU32 num_lines = rb.getNbLines();
	const PxDebugLine* lines = rb.getLines();

	glBegin(GL_LINES);

	for (PxU32 i = 0; i < num_lines; i++)
	{
		glVertex3f(lines[i].pos0.x, lines[i].pos0.y, lines[i].pos0.z);
		glVertex3f(lines[i].pos1.x, lines[i].pos1.y, lines[i].pos1.z);
	}
	glEnd();

	ModuleRenderer3D::EndDebugDraw();
}

PxRigidActor* ModulePhysX::CreateBody(const float4x4& transform, bool is_dynamic)
{
	if (!transform.IsFinite()) return nullptr;

	PxRigidActor* return_body = nullptr;
	return_body = (is_dynamic)
		? (PxRigidActor*)px_physics->createRigidDynamic(F4X4_TO_PXTRANS(transform))
		: (PxRigidActor*)px_physics->createRigidStatic(F4X4_TO_PXTRANS(transform));

	px_scene->addActor(*return_body);
	return return_body;
}

void ModulePhysX::RemoveBody(PxRigidActor* body)
{
	px_scene->removeActor(*body, true);
	body->release();
}

PxShape* ModulePhysX::CreateShape(const PxGeometry& geometry)
{
	return px_physics->createShape(geometry, *px_default_material);
}
