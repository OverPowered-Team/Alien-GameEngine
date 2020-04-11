#pragma once

#include "gpu/PxGpu.h"
#include "common/windows/PxWindowsDelayLoadHook.h"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;}
#define APP_BIN_DIR "DLLs"
#if		PX_WIN64
#define DLL_NAME_BITS "64"
#else
#define DLL_NAME_BITS "32"
#endif
#if		PX_DEBUG
#define DLL_DIR "\\physx\\_debug\\"
#elif	PX_CHECKED
#define DLL_DIR "checked\\" 
#elif	PX_PROFILE
#define DLL_DIR "profile\\" 
#else
#define DLL_DIR "\\physx\\_release\\" 
#endif

#define DEFAULT_STATIC_FRICTION 0.5f
#define DEFAULT_DYNAMIC_FRICTION 0.5f
#define DEFAULT_RESTIUTION 0.5f

using namespace physx;

class CustomDelayLoadHook : public PxDelayLoadHook
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

class CustomGpuLoadHook : public PxGpuLoadHook
{
	virtual const char* getPhysXGpuDllName() const
	{
		return "TODO";//DLLs\\physx\\debug\\PhysXGpu_32.dll";;
	}
};

class CustomErrorCallback : public PxErrorCallback
{
public:
	void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line);
};