#include "ComponentBoxCollider.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "imgui/imgui.h"

ComponentBoxCollider::ComponentBoxCollider(GameObject* go) : ComponentCollider(go)
{
	type = ComponentType::BOX_COLLIDER;
}

void ComponentBoxCollider::CreateShape(ComponentMesh* mesh)
{
	size = (mesh != nullptr) ? mesh->local_aabb.Size() :  float3::one();
	center = (mesh != nullptr) ? mesh->local_aabb.CenterPoint() : float3::zero();
	
	float3 shape_size = float3::one() * 0.5f;
	shape = new btBoxShape(btVector3(shape_size.x, shape_size.y, shape_size.z));
}

void ComponentBoxCollider::AdjustShape()
{
	scaled_center = center;
	float3 scaled_size = size.Mul(transform->GetGlobalScale().Abs());
	scaled_size = CheckInvalidCollider(scaled_size);
	shape->setLocalScaling(btVector3(scaled_size.x, scaled_size.y, scaled_size.z));
}

void ComponentBoxCollider::Reset()
{
	size.one();
	ComponentCollider::Reset();
}

void ComponentBoxCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetFloat3("size", size);
}

void ComponentBoxCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	size = to_load->GetFloat3("size");
}

float3 ComponentBoxCollider::CheckInvalidCollider(float3 size)
{
	return size.Max(float3(0.01, 0.01, 0.01));
}

bool ComponentBoxCollider::DrawInspector()
{
	ImGui::Title("Size", 1);	ImGui::DragFloat3("##size", size.ptr(), 0.1f, 0.01f, FLT_MAX);
	return true;
}

