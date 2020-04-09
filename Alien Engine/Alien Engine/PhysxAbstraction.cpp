#include "PhysxAbstraction.h"
#include "Globals.h"

PhysicsErrorCallback::PhysicsErrorCallback() {}

PhysicsErrorCallback::~PhysicsErrorCallback() {}

void PhysicsErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
{
	switch (code)
	{
	case physx::PxErrorCode::Enum::eNO_ERROR:
		LOG_ENGINE("eNO_ERROR: %s", message);
		break;
	case physx::PxErrorCode::Enum::eDEBUG_INFO: //! \brief An informational message
		LOG_ENGINE("eDEBUG_INFO: %s", message);
		break;
	case physx::PxErrorCode::Enum::eDEBUG_WARNING: //! \brief a warning message for the user to help with debugging
		LOG_ENGINE("eDEBUG_WARNING: %s", message);
		break;
	case physx::PxErrorCode::Enum::eINVALID_PARAMETER: //! \brief method called with invalid parameter(s)
		LOG_ENGINE("eINVALID_PARAMETER: %s", message);
		break;
	case physx::PxErrorCode::Enum::eINVALID_OPERATION: //! \brief method was called at a time when an operation is not possible
		LOG_ENGINE("eINVALID_OPERATION: %s", message);
		break;
	case physx::PxErrorCode::Enum::eOUT_OF_MEMORY: //! \brief method failed to allocate some memory
		LOG_ENGINE("eOUT_OF_MEMORY: %s", message);
		break;
	case physx::PxErrorCode::Enum::eINTERNAL_ERROR: //! \brief The library failed for some reason. Possibly you have passed invalid values like NaNs, which are not checked for
		LOG_ENGINE("eINTERNAL_ERROR: %s", message);
		break;
	case physx::PxErrorCode::Enum::eABORT: //! \brief An unrecoverable error, execution should be halted and log output flushed
		LOG_ENGINE("eABORT: %s", message);
		break;
	case physx::PxErrorCode::Enum::ePERF_WARNING: //! \brief The SDK has determined that an operation may result in poor performance
		LOG_ENGINE("ePERF_WARNING: %s", message);
		break;
	case physx::PxErrorCode::Enum::eMASK_ALL: //! \brief A bit mask for including all errors
		LOG_ENGINE("eMASK_ALL: %s", message);
		break;
	}
}
