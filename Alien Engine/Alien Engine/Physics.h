#pragma once
#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "UtilitiesPhysX.h"

//class ComponentCollider;
typedef unsigned int uint;

class __declspec(dllexport) Physics {
public:

	/*static void SetGravity(const float3 gravity);
	static float3 GetGravity();*/

	/*static ComponentCollider*			   RayCastClosest(math::Ray ray);
	static uint RayCastAll(Ray ray, ComponentCollider*** comp_array);
	static uint SphereCast(float3 position, float radius, ComponentCollider*** comp_array);
	static uint BoxCast(float3 size, float3 position, Quat rotation, ComponentCollider*** comp_array);*/
	//static void FreeArray(ComponentCollider*** comp_array);

	static bool Raycast(float3 origin, float3 unitDir, float maxDistance, PxRaycastBuffer& hit); // TODO: make own pxraycastbuffer data to abstract from physx
	static const std::vector<PxRaycastHit> RaycastAll(float3 origin, float3 unitDir, float maxDistance);
	static uint OverlapSphere(float3 center, float radius, ComponentCollider*** colliders);
	//static const std::vector<ComponentCollider*> OverlapSphere(float3 center, float radius);
};