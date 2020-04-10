
#include "Globals.h"
#include "Application.h"
#include "ModulePhysX.h"
#include "Optick/include/optick.h"


ModulePhysX::ModulePhysX(bool start_enabled) : Module(start_enabled)
{
	
}

ModulePhysX::~ModulePhysX()
{
}

// TODO: DELETE ME
void ModulePhysX::CreateStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for (PxU32 i = 0; i < size; i++)
	{
		for (PxU32 j = 0; j < size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}

bool ModulePhysX::loadPhysicsExplicitely()
{
	// load the dlls

	foundationLibrary = LoadLibraryA(foundationLibraryPath);
	if (!foundationLibrary)
		return false;

	commonLibrary = LoadLibraryA(commonLibraryPath);
	if (!commonLibrary)
	{
		FreeLibrary(foundationLibrary);
		return false;
	}

	physxLibrary = LoadLibraryA(physxLibraryPath);
	if (!physxLibrary)
	{
		FreeLibrary(foundationLibrary);
		FreeLibrary(commonLibrary);
		return false;
	}

	cookingLibrary = LoadLibraryA(cookingLibraryPath);
	if (!cookingLibrary)
	{
		FreeLibrary(foundationLibrary);
		FreeLibrary(commonLibrary);
		FreeLibrary(physxLibrary);
		return false;
	}

	//// get the function pointers
	//s_PxCreateFoundation_Func = (PxCreateFoundation_FUNC*)GetProcAddress(foundationLibrary, "PxCreateFoundation");
	//s_PxCreatePhysics_Func = (PxCreatePhysics_FUNC*)GetProcAddress(physxLibrary, "PxCreateBasePhysics");
	//s_PxSetPhysXDelayLoadHook_Func = (PxSetPhysXDelayLoadHook_FUNC*)GetProcAddress(physxLibrary, "PxSetPhysXDelayLoadHook");
	//s_PxSetPhysXCommonDelayLoadHook_Func = (PxSetPhysXCommonDelayLoadHook_FUNC*)GetProcAddress(commonLibrary, "PxSetPhysXCommonDelayLoadHook");

	//s_PxSetPhysXGpuLoadHook_Func = (PxSetPhysXGpuLoadHook_FUNC*)GetProcAddress(physxLibrary, "PxSetPhysXGpuLoadHook");
	//s_PxGetSuggestedCudaDeviceOrdinal_Func = (PxGetSuggestedCudaDeviceOrdinal_FUNC*)GetProcAddress(physxLibrary, "PxGetSuggestedCudaDeviceOrdinal");
	//s_PxCreateCudaContextManager_Func = (PxCreateCudaContextManager_FUNC*)GetProcAddress(physxLibrary, "PxCreateCudaContextManager");

	//// check if we have all required function pointers
	//if (s_PxCreateFoundation_Func == NULL || s_PxCreatePhysics_Func == NULL || s_PxSetPhysXDelayLoadHook_Func == NULL || s_PxSetPhysXCommonDelayLoadHook_Func == NULL)
	//	return false;

	//if (s_PxSetPhysXGpuLoadHook_Func == NULL || s_PxGetSuggestedCudaDeviceOrdinal_Func == NULL || s_PxCreateCudaContextManager_Func == NULL)
	//	return false;
	return true;
}

bool ModulePhysX::Init()
{
	OPTICK_EVENT();
	LOG_ENGINE("Creating 3D PhysX world");
	bool ret = true;

	// load the explictely named dlls
	const bool isLoaded = loadPhysicsExplicitely(); // load debug or release dlls
	if (!isLoaded)
		return false;

	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback); // TODO: implement custom error callback

	// set PhysX and PhysXCommon delay load hook, this must be done before the create physics is called, before ----
	// the PhysXFoundation, PhysXCommon delay load happens.
	customDelayLoadHook delayLoadHook;
	PxSetPhysXDelayLoadHook(&delayLoadHook);
	PxSetPhysXCommonDelayLoadHook(&delayLoadHook);
	PxSetPhysXCookingDelayLoadHook(&delayLoadHook);

	// set PhysXGpu load hook
	customGpuLoadHook gpuLoadHook;
	PxSetPhysXGpuLoadHook(&gpuLoadHook);
	// --------------------------------------------------------------------------------------------------------------

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*groundPlane);

	for (PxU32 i = 0; i < 5; i++)
		CreateStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);

	/*if (!interactive)
		createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));*/

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
	static bool first_frame_playing = true;
	OPTICK_EVENT();

	gScene->simulate(1.0f / 60.0f); // TODO, fixed time step / substeps
	gScene->fetchResults(true);

	return UPDATE_CONTINUE;
}

update_status ModulePhysX::PostUpdate(float dt)
{
	
	return UPDATE_CONTINUE;
}

bool ModulePhysX::CleanUp()
{

	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);

	unloadPhysicsExplicitely();

	return true;
}

// unload the dlls
void ModulePhysX::unloadPhysicsExplicitely()
{
	FreeLibrary(physxLibrary);
	FreeLibrary(commonLibrary);
	FreeLibrary(foundationLibrary);
}

