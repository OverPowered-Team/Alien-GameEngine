#include "ComponentBoxCollider.h"
#include "ComponentMesh.h"
#include "GameObject.h"

ComponentBoxCollider::ComponentBoxCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Box Collider");
	type = ComponentType::BOX_COLLIDER;
	size = float3::one;
}

void ComponentBoxCollider::CreateShape(C_Mesh* mesh)
{
	if (is_loaded == false)
	{
		size = (mesh != nullptr) ? mesh->mesh_aabb.Size() : size = float3::one;
		center = (mesh != nullptr) ? mesh->mesh_aabb.CenterPoint() : float3::zero;
	}
	float3 shape_size = float3::one * 0.5f;
	shape = new btBoxShape(btVector3(shape_size.x, shape_size.y, shape_size.z));
}

void ComponentBoxCollider::AdjustShape()
{
	scaled_center = center;

	float3 scaled_size = size.Mul(linked_go->transform->scale.Abs());
	scaled_size = CheckInvalidCollider(scaled_size);
	shape->setLocalScaling(btVector3(scaled_size.x, scaled_size.y, scaled_size.z));
}

void ComponentBoxCollider::SaveCollider(Config& config)
{
	config.AddFloatArray("size", (float*)&size, 3);
}

void ComponentBoxCollider::LoadCollider(Config& config)
{
	size = config.GetFloat3("size", { 1.f ,1.f, 1.f });
}

float3 ComponentBoxCollider::CheckInvalidCollider(float3 size)
{
	return size.Max(float3(0.01, 0.01, 0.01));
}

void ComponentBoxCollider::DrawPanelColliderInfo()
{
	ImGui::Title("Size", 1);	ImGui::DragFloat3("##size", size.ptr(), 0.1f, 0.01f, FLT_MAX);
}

