#include "Application.h"
#include "ComponentPhysics.h"
#include "ComponentBoxCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ModulePhysics.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "mmgr/mmgr.h"

ComponentBoxCollider::ComponentBoxCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Box Collider");
	type = ComponentType::BOX_COLLIDER;
	shape = App->physx->CreateShape(PxBoxGeometry( .5f ,.5f , .5f ));

	App->SendAlienEvent(this, AlienEventType::COLLIDER_ADDED);
}

void ComponentBoxCollider::SetSize(const float3 value)
{
	if (value.Equals(size)) return;
	size = value;
	PxBoxGeometry geo(F3_TO_PXVEC3(size * 0.5f));
	physics->RemoveCollider(this);
	shape->setGeometry(geo);
	physics->AddCollider(this);
}


void ComponentBoxCollider::DrawSpecificInspector()
{
	float3 current_size = size;

	ImGui::Title("Size", 1);		
	if (ImGui::DragFloat3("##size", current_size.ptr(), 0.1f, 0.01f, FLT_MAX)) {
		SetSize(current_size);
	};

}

void ComponentBoxCollider::CreateDefaultShape()
{
}

void ComponentBoxCollider::Clone(Component* clone)
{
	ComponentBoxCollider* box_clone = (ComponentBoxCollider*)clone;
	center = box_clone->GetCenter();
	size = box_clone->GetSize();
}

void ComponentBoxCollider::Reset()
{
	ComponentCollider::Reset();
	SetSize(float3::one());
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
