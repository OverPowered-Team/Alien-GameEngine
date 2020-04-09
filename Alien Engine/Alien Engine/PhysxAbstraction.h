#pragma once

#include "PhysX\include\PxPhysicsAPI.h"
#include "PhysX\include\extensions\PxDefaultAllocator.h"
#include "PhysX\include\extensions\PxDefaultCpuDispatcher.h"     

#define DEFAULT_STATIC_FRICTION 0.5f
#define DEFAULT_DYNAMIC_FRICTION 0.5f
#define DEFAULT_RESTIUTION 0.5f

class PhysicsErrorCallback : public physx::PxErrorCallback
{
public:

	PhysicsErrorCallback();
	~PhysicsErrorCallback();

	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line);
};

