#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "ComponentMesh.h"
#include "GameObject.h"

ComponentCollider::ComponentCollider(GameObject* go) : Component(go)
{
	Reset();
	ComponentMesh* mesh = game_object_attached->GetComponent<ComponentMesh>();
	CreateShape(mesh);
}

void ComponentCollider::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetFloat3("Center", center);

}

void ComponentCollider::LoadComponent(JSONArraypack* to_load)
{
	Reset();
	center = to_load->GetFloat3("Center");
}

void ComponentCollider::Update()
{
	// Match Size Scalling ----------------------------------

	AdjustShape();

	// Get GameObject Components ----------------------------
	transform = (transform == nullptr) ? game_object_attached->GetComponent<ComponentTransform>() : transform;

	// Body Logic -------------------------------------------

	float3 world_center = GetWorldCenter();
}

void ComponentCollider::Render()
{
	if (shape == nullptr) return;

	if (game_object_attached->IsSelected()/* && App->scene->editor_mode*/)
	{
		App->physics->RenderCollider(this);
	}
}

void ComponentCollider::Reset()
{
	center.zero();
	scaled_center.zero();
}

bool ComponentCollider::DrawInspector()
{
	ImGui::Title("Center", 1);			ImGui::DragFloat3("##center", center.ptr(), 0.1f);

	return true;
}

float3 ComponentCollider::GetWorldCenter()
{
	return transform->GetGlobalRotation().Mul(scaled_center);
}

void ComponentCollider::AddToRigidBody()
{
	if (rigid_body == nullptr)
	{
		game_object_attached->AddComponent(new ComponentRigidBody(game_object_attached));
		rigid_body = game_object_attached->GetComponent<ComponentRigidBody>();
	}
	center = float3::zero();
	rotation = Quat::identity();
	
	rigid_body->compound_shape->addChildShape(ToBtTransform(center, rotation), shape);
}

void ComponentCollider::RemoveFromRigidBody()
{
	rigid_body->compound_shape->removeChildShape(shape);
}
