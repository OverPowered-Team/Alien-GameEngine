#include "ComponentBoxCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ModulePhysics.h"
#include "GameObject.h"
#include "imgui/imgui.h"

ComponentBoxCollider::ComponentBoxCollider(GameObject* go) : ComponentCollider(go)
{
	type = ComponentType::BOX_COLLIDER;
	size = float3::one();
	center = float3::zero();

	if (!game_object_attached->HasComponent(ComponentType::RIGID_BODY))
	{
		game_object_attached->AddComponent(new ComponentRigidBody(game_object_attached));
	}

	rigid_body = game_object_attached->GetComponent<ComponentRigidBody>();

	CreateShape();
	AdjustShapeToMesh();


}

void ComponentBoxCollider::CreateShape() 
{
	//size = (mesh != nullptr) ? mesh->local_aabb.Size() :  float3::one();
	//center = (mesh != nullptr) ? mesh->local_aabb.CenterPoint() : float3::zero();
	
	if (shape != nullptr)
	{
		rigid_body->RemoveCollider(this);
		delete shape;
	}

	float3 scaled_size = size.Mul(transform->GetGlobalScale().Abs());
	scaled_size = CheckInvalidCollider(scaled_size);
	float3 final_size = scaled_size * 0.5f;
	shape = new btBoxShape(ToBtVector3(final_size));
	shape->setUserPointer(this);
	rigid_body->AddCollider(this);
}

void ComponentBoxCollider::AdjustShapeToMesh()
{
	ComponentMesh* mesh = game_object_attached->GetComponent<ComponentMesh>();

	if (mesh != nullptr) 
	{
		center = mesh->local_aabb.CenterPoint();
		size = mesh->local_aabb.Size();
		CreateShape();
	}
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
	static float3 last_scale = transform->GetGlobalScale();
	float3 last_center = center;
	float3 last_size = size;

	ComponentCollider::DrawInspector();

	if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();
		ImGui::Title("Center", 1);			ImGui::DragFloat3("##center", center.ptr(), 0.1f);
		ImGui::Title("Size", 1);			ImGui::DragFloat3("##size", size.ptr(), 0.1f, 0.01f, FLT_MAX);
		ImGui::Spacing();
	}

	if (!last_scale.Equals( transform->GetGlobalScale()) || !last_size.Equals(size) || !last_center.Equals(center))
	{
		last_scale = transform->GetGlobalScale();
		scaled_center = center.Mul(last_scale);
		CreateShape();
	}


	return true;
}

