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
	shape = App->physx->CreateShape(PxBoxGeometry(.5f, .5f, .5f));
	InitCollider();
}

void ComponentBoxCollider::SetSize(const float3 value)
{
	size = value;
	PxBoxGeometry geo(F3_TO_PXVEC3(size.Mul(physics->scale) * 0.5f));
	BeginUpdateShape();
	shape->setGeometry(geo);
	EndUpdateShape();
}

void ComponentBoxCollider::QueryMesh(ComponentMesh* mesh)
{
	if (mesh == nullptr) return;
	BeginUpdateShape(true);
	SetSize(mesh->local_aabb.Size());
	SetCenter(mesh->local_aabb.CenterPoint());
	BeginUpdateShape(true);
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
	float3 current_size = size;

	ImGui::Title("Size", 1);
	if (ImGui::DragFloat3("##size", current_size.ptr(), 0.1f, 0.01f, FLT_MAX)) {
		SetSize(current_size);
	};
}

void ComponentBoxCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetFloat3("Size", size);
}

void ComponentBoxCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	SetSize(to_load->GetFloat3("Size"));
}

void ComponentBoxCollider::Clone(Component* clone)
{
	ComponentBoxCollider* box_clone = (ComponentBoxCollider*)clone;
	BeginUpdateShape(true);
	SetCenter(box_clone->center);
	SetSize(box_clone->size);
	EndUpdateShape(true);
}

void ComponentBoxCollider::Reset()
{
	BeginUpdateShape(true);
	SetCenter(float3::zero());
	SetSize(float3::one());
	EndUpdateShape(true);
}