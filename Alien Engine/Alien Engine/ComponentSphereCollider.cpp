#include "Application.h"
#include "ComponentPhysics.h"
#include "ComponentSphereCollider.h"
#include "ComponentTransform.h"
#include "ModulePhysics.h"
#include "GameObject.h"
#include "imgui/imgui.h"

ComponentSphereCollider::ComponentSphereCollider(GameObject* go) :ComponentCollider(go)
{
	name.assign("Sphere Collider");
	type = ComponentType::SPHERE_COLLIDER;
	shape = App->physx->CreateShape(PxSphereGeometry(.5f));
	App->SendAlienEvent(this, AlienEventType::COLLIDER_ADDED);
}

void ComponentSphereCollider::SetRadius(float value)
{
	if (value == radius) return;
	radius = value;
	PxSphereGeometry geo(radius);
	physics->RemoveCollider(this);
	shape->setGeometry(geo);
	physics->AddCollider(this);
}

void ComponentSphereCollider::DrawSpecificInspector()
{
	float current_radius = radius;
	ImGui::Title("Radius", 1);	if (ImGui::DragFloat("##radius", &current_radius, 0.1f, 0.01f, FLT_MAX) ){ SetRadius(current_radius); }
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

