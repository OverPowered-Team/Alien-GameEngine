#include "Physics.h"
#include "Application.h"
#include "ModulePhysX.h"

void Physics::SetGravity(const float3 gravity)
{
	App->physx->SetGravity(gravity);
}

float3 Physics::GetGravity()
{
	return App->physx->GetGravity();
}

bool Physics::Raycast(float3 origin, float3 unit_dir, float max_dist, int layer_mask)
{
	App->physx->layer_mask = layer_mask;
	bool ret = App->physx->Raycast(origin, unit_dir, max_dist);
	App->physx->layer_mask = -1;
	return ret;
}

bool Physics::Raycast(float3 origin, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	App->physx->layer_mask = layer_mask;
	bool ret = App->physx->Raycast(origin, unit_dir, max_dist, hit);
	App->physx->layer_mask = -1;
	return ret;
}

const std::vector<RaycastHit>& Physics::RaycastAll(float3 origin, float3 unit_dir, float max_dist, int layer_mask)
{
	static std::vector<RaycastHit> ret;
	App->physx->layer_mask = layer_mask;
	ret = App->physx->RaycastAll(origin, unit_dir, max_dist);
	App->physx->layer_mask = -1;
	return ret;
}

// TODO: uncomment this when we have heap change integred
const std::vector<ComponentCollider*>& Physics::OverlapSphere(float3 center, float radius)
{
	static  std::vector<ComponentCollider*> ret;
	ret = App->physx->OverlapSphere(center, radius);
	return ret;
}
