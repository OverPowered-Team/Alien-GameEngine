#pragma once
#include "gpu/PxGpu.h"
#include "common/windows/PxWindowsDelayLoadHook.h"
#include "foundation/PxTransform.h"
#include "foundation/PxVec3.h"
#include "foundation/PxQuat.h"
#include "PhysX/include/characterkinematic/PxExtended.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/Quat.h"

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

inline float3			PXVEC3EXT_TO_F3(const PxExtendedVec3 & vec) { return  float3(vec.x, vec.y, vec.z); }
inline PxExtendedVec3	F3_TO_PXVEC3EXT(const float3 & vec) { return  PxExtendedVec3(vec.x, vec.y, vec.z); }
inline PxVec3			F3_TO_PXVEC3(const float3 & vec) { return  PxVec3(vec.x, vec.y, vec.z); }
inline float3			PXVEC3_TO_F3(const PxVec3 & vec) { return  float3(vec.x, vec.y, vec.z); }
inline PxQuat			QUAT_TO_PXQUAT(const Quat & quat) { return  PxQuat(quat.x, quat.y, quat.z, quat.w); }
inline Quat				PXQUAT_TO_QUAT(const PxQuat & quat) { return  Quat(quat.x, quat.y, quat.z, quat.w); }
inline PxTransform		F4X4_TO_PXTRANS(const float4x4 & trans) { return PxTransform(F3_TO_PXVEC3(trans.TranslatePart()), QUAT_TO_PXQUAT(trans.RotatePart().RemoveScale2().ToQuat())); }
inline float4x4			PXTRANS_TO_F4X4(const PxTransform & trans) { return float4x4(PXQUAT_TO_QUAT(trans.q), PXVEC3_TO_F3(trans.p)); }

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

