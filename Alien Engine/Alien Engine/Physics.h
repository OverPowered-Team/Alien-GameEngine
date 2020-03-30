#pragma once
#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Geometry/Ray.h"

class ComponentCollider;

class __declspec(dllexport) Physics {
public:

	static std::vector<ComponentCollider*> RayCastAll(Ray ray);
	static ComponentCollider* RayCastClosest(math::Ray ray);
	static std::vector<ComponentCollider*>  SphereCast(float3 position, float radius);

};

