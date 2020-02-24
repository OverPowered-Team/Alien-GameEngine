#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ReturnZ.h"

ComponentCollider::ComponentCollider(GameObject* go) : Component(go)
{
	Reset();
	// Get GameObject Components ----------------------------
	transform = (transform == nullptr) ? game_object_attached->GetComponent<ComponentTransform>() : transform;

}

void ComponentCollider::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetFloat3("Center", center);
}

void ComponentCollider::LoadComponent(JSONArraypack* to_load)
{
	center = to_load->GetFloat3("Center");
}

void ComponentCollider::Update()
{
	// Match Size Scalling ----------------------------------
	float3 world_center = GetWorldCenter();
}

void ComponentCollider::DrawCollider()
{
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
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}

	ImGui::PopID();
	ImGui::SameLine();

	return true;
}

float3 ComponentCollider::GetWorldCenter()
{
	return transform->GetGlobalRotation().Mul(scaled_center);
}

