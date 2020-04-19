#include "Physics.h"
#include "Application.h"
#include "ModulePhysX.h"

bool Physics::Raycast(float3 origin, float3 unitDir, float maxDistance, PxRaycastBuffer& hit)
{
	return App->physx->Raycast(origin, unitDir, maxDistance, hit);
}

uint Physics::OverlapSphere(float3 center, float radius, ComponentCollider*** colliders)
{
	std::vector<ComponentCollider*> _colliders = App->physx->OverlapSphere(center, radius);

	if (_colliders.size() > 0)
	{
		(*colliders) = new ComponentCollider * [_colliders.size()]; // TODO: CHANGE THIS ASAP to return a simple vector filled with colliders instead this
		for (uint i = 0; i < _colliders.size(); ++i)
			(*colliders)[i] = _colliders[i];
	}

	return _colliders.size();
}

// TODO: uncomment this when we have heap change integred
//const std::vector<ComponentCollider*> Physics::OverlapSphere(float3 center, float radius)
//{
//	return App->physx->OverlapSphere(center, radius);
//}

//void Physics::SetGravity(const float3 gravity)
//{
//	App->physics->SetGravity(gravity);
//}
//
//float3 Physics::GetGravity()
//{
//	return App->physics->GetGravity();
//}

//uint Physics::RayCastAll(Ray ray, ComponentCollider*** comp_array)
//{
//	std::vector<ComponentCollider*> found = App->physics->RayCastAll(ray);
//	if (!found.empty()) {
//		(*comp_array) = new ComponentCollider * [found.size()];
//		for (uint i = 0; i < found.size(); ++i) {
//			(*comp_array)[i] = found[i];
//		}
//	}
//	return found.size();
//}
//
//ComponentCollider* Physics::RayCastClosest(math::Ray ray)
//{
//	return App->physics->RayCastClosest(ray);
//}
//
//uint Physics::SphereCast(float3 position, float radius, ComponentCollider*** comp_array)
//{
//	std::vector<ComponentCollider*> found = App->physics->SphereCast(position, radius);
//	if (!found.empty()) {
//		(*comp_array) = new ComponentCollider *[found.size()];
//		for (uint i = 0; i < found.size(); ++i) {
//			(*comp_array)[i] = found[i];
//		}
//	}
//	return found.size();
//}
//
//uint Physics::BoxCast(float3 size, float3 position, Quat rotation, ComponentCollider*** comp_array)
//{
//	auto found = App->physics->BoxCast(size, position, rotation);
//	if (!found.empty()) {
//		(*comp_array) = new ComponentCollider * [found.size()];
//		for (uint i = 0; i < found.size(); ++i) {
//			(*comp_array)[i] = found[i];
//		}
//	}
//	return found.size();
//}
//
//void Physics::FreeArray(ComponentCollider*** comp_array)
//{
//	delete[] * comp_array;
//}
