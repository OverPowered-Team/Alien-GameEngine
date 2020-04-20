#include "Application.h"
#include "ComponentPhysics.h"
#include "ComponentBoxCollider.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ModulePhysics.h"
#include "GameObject.h"
#include "imgui/imgui.h"

ComponentBoxCollider::ComponentBoxCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Box Collider");
	type = ComponentType::BOX_COLLIDER;
	shape = App->physx->CreateShape(PxBoxGeometry( .5f ,.5f , .5f ));
	InitCollider();
}

const float3 ComponentBoxCollider::CalculateSize()
{
	return size.Mul(transform->GetGlobalScale()) * 0.5f;
}

void ComponentBoxCollider::ScaleChanged()
{
	bool update_forced = true;
	BeginUpdateShape(update_forced); // Force Update to no attach and deattach too times
	SetSize(size);
	SetCenter(center);
	EndUpdateShape(update_forced);
}

void ComponentBoxCollider::DrawSpecificInspector()
{
	ImGui::Title("Size", 1);		
	if (ImGui::DragFloat3("##size", size.ptr(), 0.1f, 0.01f, FLT_MAX)) {
		ReCreateBoxShape();
	};
}

void ComponentBoxCollider::Clone(Component* clone)
{
	ComponentBoxCollider* box_clone = (ComponentBoxCollider*)clone;
	center = box_clone->GetCenter();
	size = box_clone->CalculateSize();
}

void ComponentBoxCollider::Reset()
{
	ComponentCollider::Reset();
	size = GetLocalMeshAabbSize();
	ReCreateBoxShape();
}

void ComponentBoxCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetFloat3("Size", size);
}

void ComponentBoxCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	size = to_load->GetFloat3("Size");
	ReCreateBoxShape();
}

// * --------- ACCESS THROUGH SCRIPTING ----------* //

void ComponentBoxCollider::SetSize(float3 size)
{
	this->size = size;
	ReCreateBoxShape();
}