#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
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

	if (Time::GetGameState() == Time::GameState::PAUSE) return;

	SetBouncing(bouncing);
	SetFriction(friction);
	SetAngularFriction(angular_friction);

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
	bool last_is_trigger = is_trigger;

	ImGui::Spacing();

	ImGui::Title("Is Trigger", 1);		ImGui::Checkbox("##is_trigger", &last_is_trigger);
	ImGui::Title("Center", 1);			ImGui::DragFloat3("##center", center.ptr(), 0.1f);

	// Draw Collider Adiional Info ---------------

	DrawInspectorCollider();

	ImGui::Spacing();
	ImGui::Title("Physic Material", 1); ImGui::Text("");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Title("Bouncing", 2);	    ImGui::DragFloat("##bouncing", &bouncing, 0.01f, 0.00f, 1.f);
	ImGui::Title("Linear Fric.", 2);	ImGui::DragFloat("##friction", &friction, 0.01f, 0.00f, FLT_MAX);
	ImGui::Title("Angular Fric.", 2);	ImGui::DragFloat("##angular_friction", &angular_friction, 0.01f, 0.00f, FLT_MAX);

	ImGui::Spacing();

	if (last_is_trigger != is_trigger)
	{
		SetIsTrigger(last_is_trigger);
	}

	return true;
}

void ComponentCollider::SetIsTrigger(bool value)
{

}


void ComponentCollider::SetBouncing(float& bouncing)
{
	if (bouncing != aux_body->getRestitution())
	{
		float min = 0.f, max = 1.f;
		math::Clamp(&bouncing, &min, &max);
		aux_body->setRestitution(bouncing);
	}
}

void ComponentCollider::SetFriction(float& friction)
{
	if (friction != aux_body->getFriction())
	{
		aux_body->setFriction(friction);
	}
}

void ComponentCollider::SetAngularFriction(float& angular_drag)
{
	if (angular_drag != aux_body->getRollingFriction())
	{
		aux_body->setRollingFriction(angular_drag);
	}
}

bool ComponentCollider::GetIsTrigger()
{
	return is_trigger;
}

float3 ComponentCollider::GetWorldCenter()
{
	return transform->GetGlobalRotation().Mul(scaled_center);
}

void ComponentCollider::AddToRigidBody()
{
	C_RigidBody* rigid_body = game_object_attached->GetComponent<C_RigidBody>();
}

void ComponentCollider::RemoveFromRigidBody()
{
}
