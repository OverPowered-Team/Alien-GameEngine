#include "Physics.h"
#include "Application.h"
#include "ModulePhysics.h"

std::vector<ComponentCollider*> Physics::RayCastAll(Ray ray)
{
	return App->physics->RayCastAll(ray);
}

ComponentCollider* Physics::RayCastClosest(math::Ray ray)
{
	return App->physics->RayCastClosest(ray);
}

std::vector<ComponentCollider*> Physics::SphereCast(float3 position, float radius)
{
	return App->physics->SphereCast(position, radius);
}
