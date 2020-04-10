#ifndef __MODULE_PHYSX_H__
#define __MODULE_PHYSX_H__

#include "Module.h"
#include "PxPhysicsAPI.h"

#include "common/windows/PxWindowsDelayLoadHook.h"

using namespace physx;

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

// delay load hook define helpers
#define APP_BIN_DIR "DLLs"
#if PX_WIN64
	#define DLL_NAME_BITS "64"
#else
#define DLL_NAME_BITS "32"
#endif
#if PX_DEBUG
	#define DLL_DIR "\\physx\\debug\\"
#elif PX_CHECKED
	#define DLL_DIR "checked\\" 
#elif PX_PROFILE
	#define DLL_DIR "profile\\" 
#else
#define DLL_DIR "\\physx\\release\\" 
#endif

// TODO: rework custom delayhooks
class customDelayLoadHook : public PxDelayLoadHook
{
	virtual const char* getPhysXCommonDllName() const
	{
		return "TODO";//DLLs\\physx\\debug\\PhysXCommon_32.dll";
	}

	virtual const char* getPhysXFoundationDllName() const
	{
		return "TODO";//DLLs\\physx\\debug\\PhysXFoundation_32.dll";
	}
};

class customGpuLoadHook : public PxGpuLoadHook
{
	virtual const char* getPhysXGpuDllName() const
	{
		return "TODO";//DLLs\\physx\\debug\\PhysXGpu_32.dll";;
	}
};

class ModulePhysX : public Module
{
	

public:
	ModulePhysX(bool start_enabled = true);
	~ModulePhysX();


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
	// TODO: move this block to another header (and defines up there) ---------------------------------------------------------------
	// typedef the PhysX entry points
	//typedef PxFoundation* (PxCreateFoundation_FUNC)(PxU32, PxAllocatorCallback&, PxErrorCallback&);
	//typedef PxPhysics* (PxCreatePhysics_FUNC)(PxU32, PxFoundation&, const PxTolerancesScale& scale, bool, PxPvd*);
	//typedef void (PxSetPhysXDelayLoadHook_FUNC)(const PxDelayLoadHook* hook);
	//typedef void (PxSetPhysXCommonDelayLoadHook_FUNC)(const PxDelayLoadHook* hook);
	//typedef void (PxSetPhysXGpuLoadHook_FUNC)(const PxGpuLoadHook* hook);
	//typedef int (PxGetSuggestedCudaDeviceOrdinal_FUNC)(PxErrorCallback& errc);
	//typedef PxCudaContextManager* (PxCreateCudaContextManager_FUNC)(PxFoundation& foundation, const PxCudaContextManagerDesc& desc, physx::PxProfilerCallback* profilerCallback);

	//// set the function pointers to NULL
	//PxCreateFoundation_FUNC* s_PxCreateFoundation_Func = NULL;
	//PxCreatePhysics_FUNC* s_PxCreatePhysics_Func = NULL;
	//PxSetPhysXDelayLoadHook_FUNC* s_PxSetPhysXDelayLoadHook_Func = NULL;
	//PxSetPhysXCommonDelayLoadHook_FUNC* s_PxSetPhysXCommonDelayLoadHook_Func = NULL;
	//PxSetPhysXGpuLoadHook_FUNC* s_PxSetPhysXGpuLoadHook_Func = NULL;
	//PxGetSuggestedCudaDeviceOrdinal_FUNC* s_PxGetSuggestedCudaDeviceOrdinal_Func = NULL;
	//PxCreateCudaContextManager_FUNC* s_PxCreateCudaContextManager_Func = NULL;

	bool loadPhysicsExplicitely();
	void unloadPhysicsExplicitely();
	// delay loaded dlls
	HMODULE foundationLibrary = NULL;
	HMODULE commonLibrary = NULL;
	HMODULE physxLibrary = NULL;
	const char* foundationLibraryPath = APP_BIN_DIR DLL_DIR "PhysXFoundation_" DLL_NAME_BITS ".dll";
	const char* commonLibraryPath = APP_BIN_DIR DLL_DIR "PhysXCommon_" DLL_NAME_BITS ".dll";
	const char* physxLibraryPath = APP_BIN_DIR DLL_DIR "PhysX_" DLL_NAME_BITS ".dll";
	const char* gpuLibraryPath = APP_BIN_DIR DLL_DIR "PhysXGpu_" DLL_NAME_BITS ".dll";

	// ----------------------------------------------------------------------------------------------------------------------------

private:
	
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;

	PxFoundation* gFoundation = nullptr;
	PxPhysics* gPhysics = nullptr;

	PxDefaultCpuDispatcher* gDispatcher = nullptr;
	PxScene* gScene = nullptr;

	PxMaterial* gMaterial = nullptr;

	PxPvd* gPvd = nullptr;

	PxReal stackZ = 10.0f;
	
public:
};

#endif // !__MODULE_PHYSX_H__
