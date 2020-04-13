#include "ComponentSphereCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "mmgr/mmgr.h"

ComponentSphereCollider::ComponentSphereCollider(GameObject* go) :ComponentCollider(go)
{
	name.assign("Sphere Collider");
	type = ComponentType::SPHERE_COLLIDER;
}

void ComponentSphereCollider::SetRadius(float value)
{
}


void ComponentSphereCollider::CreateDefaultShape()
{

}

void ComponentSphereCollider::DrawSpecificInspector()
{
	float current_radius = radius;
	ImGui::Title("Radius", 1);	ImGui::DragFloat("##radius", &current_radius, 0.1f, 0.01f, FLT_MAX);
	SetRadius(current_radius);
}

void ComponentSphereCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetNumber("Radius", radius);
}

void ComponentSphereCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	SetRadius(to_load->GetNumber("Radius"));
}

