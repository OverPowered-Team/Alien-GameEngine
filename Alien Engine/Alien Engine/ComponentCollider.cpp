#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "ComponentPhysics.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ReturnZ.h"
#include "Time.h"
#include "Event.h"

ContactPoint::ContactPoint(const float3& normal, const float3& point, float separation, ComponentCollider* this_collider, ComponentCollider* other_collider) :
	normal(normal), point(point), separation(separation), this_collider(this_collider), other_collider(other_collider) {}

Collision::Collision(ComponentCollider* collider, ComponentRigidBody* rigid_body, ComponentTransform* transform, const std::vector<ContactPoint>& contancts, 
	uint num_contact, GameObject* game_object, const float3& impulse, const float3& relative_velocity) :
	collider(collider), rigid_body(rigid_body), transform(transform), contancts(contancts), num_contact(num_contact), game_object(game_object), impulse(impulse) , relative_velocity(relative_velocity){}

ComponentCollider::ComponentCollider(GameObject* go) : ComponentBasePhysic(go)
{
	// Default values 
	center = float3::zero();
	rotation = float3::zero();
}

ComponentCollider::~ComponentCollider()
{
	if (!IsController()) {
		App->SendAlienEvent(this, AlienEventType::COLLIDER_DELETED);
		shape->release();
		shape = nullptr;
	}
}

// Colliders Functions --------------------------------

void ComponentCollider::SetCenter(const float3& value)
{
	if (center.Equals(value)) return;
	center = value;
	PxTransform trans = shape->getLocalPose();
	trans.p = F3_TO_PXVEC3(center);
	BeginUpdateShape();
	shape->setLocalPose(trans);
	EndUpdateShape();
}

void ComponentCollider::SetRotation(const float3& value)
{
	if (rotation.Equals(value)) return;
	rotation = value;
	PxTransform trans = shape->getLocalPose();
	float3 rad_rotation = DEGTORAD * rotation;
	trans.q = QUAT_TO_PXQUAT(Quat::FromEulerXYZ(rad_rotation.x, rad_rotation.y, rad_rotation.z));
	BeginUpdateShape();
	shape->setLocalPose(trans);
	EndUpdateShape();
}

void ComponentCollider::SetIsTrigger(bool value)
{
	is_trigger = value;

	BeginUpdateShape();
	if (is_trigger) {
		shape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, false);
		shape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, true);
	}
	else {
		shape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, false);
		shape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, true);
	}
	EndUpdateShape();
}

void ComponentCollider::SetBouncing(const float value)
{

}

void ComponentCollider::SetFriction(const float value)
{

}

void ComponentCollider::SetAngularFriction(const float value)
{

}

void ComponentCollider::SetCollisionLayer(std::string layer)
{
	int index = 0;
	if (!physics->layers->GetIndexByName(layer, index)) return;
	layer_num = index;
	layer_name = layer;
	BeginUpdateShape();
	PxFilterData filter_data;
	filter_data.word0 = index;
	filter_data.word1 = game_object_attached->ID;
	shape->setSimulationFilterData(filter_data);
	EndUpdateShape();

	physics->WakeUp();
}

std::string ComponentCollider::GetCollisionLayer()
{
	return layer_name;
}

void ComponentCollider::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);

	to_save->SetFloat3("Center", center);
	to_save->SetFloat3("Rotation", rotation);
	to_save->SetBoolean("IsTrigger", is_trigger);
	to_save->SetNumber("Bouncing", bouncing);
	to_save->SetNumber("Friction", friction);
	to_save->SetNumber("AngularFriction", angular_friction);
	to_save->SetString("CollisionLayer", layer_name.c_str());
}

void ComponentCollider::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");

	if (enabled == false)
	{
		OnDisable();
	}

	SetCenter(to_load->GetFloat3("Center"));
	SetRotation(to_load->GetFloat3("Rotation"));
	SetIsTrigger(to_load->GetBoolean("IsTrigger"));
	SetBouncing(to_load->GetNumber("Bouncing"));
	SetFriction(to_load->GetNumber("Friction"));
	SetAngularFriction(to_load->GetNumber("AngularFriction"));
	SetCollisionLayer(to_load->GetString("CollisionLayer"));
}

void ComponentCollider::Update()
{
}

void ComponentCollider::OnEnable()
{
	App->SendAlienEvent(this, AlienEventType::COLLIDER_ENABLED);
}

void ComponentCollider::OnDisable()
{
	App->SendAlienEvent(this, AlienEventType::COLLIDER_DISABLED);
}

void ComponentCollider::DrawScene()
{
	if (enabled == true && (game_object_attached->IsSelected() || App->physx->debug_physics ))
	{
		App->physx->DrawCollider(this);
	}
}

void ComponentCollider::Reset()
{
	SetCenter(float3::zero());
	SetIsTrigger(false);
	SetBouncing(0.1f);
	SetFriction(0.5f);
	SetAngularFriction(0.1f);
}

bool ComponentCollider::DrawInspector()
{
	static bool check;
	check = enabled;

	ImGui::PushID(this);

	if (ImGui::Checkbox("##CmpActive", &check)) {
		enabled = check;
		if (!enabled)
			OnDisable();
		else
			OnEnable();
	}

	ImGui::SameLine();

	if (ImGui::CollapsingHeader(name.c_str(), &not_destroy))
	{
		ImGui::Spacing();

		DrawLayersCombo();

		float3 current_center = center;
		float3 current_rotation = rotation;
		bool current_is_trigger = is_trigger;
		float current_bouncing = bouncing;
		float current_friction = friction;
		float current_angular_friction = angular_friction;

		ImGui::Title("Center", 1);			if (ImGui::DragFloat3("##center", current_center.ptr(), 0.05f)) { SetCenter(current_center); }
		ImGui::Title("Rotation", 1);			if (ImGui::DragFloat3("##rotation", current_rotation.ptr(), 0.2f)) { SetRotation(current_rotation); }

		DrawSpecificInspector();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Is Trigger", 1);		if (ImGui::Checkbox("##is_trigger", &current_is_trigger)) { SetIsTrigger(current_is_trigger); }
		ImGui::Spacing();
		ImGui::Title("Physic Material", 1); ImGui::Text("");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Bouncing", 2);	    if (ImGui::DragFloat("##bouncing", &current_bouncing, 0.01f, 0.00f, 1.f)) { SetBouncing(current_bouncing); }
		ImGui::Title("Linear Fric.", 2);	if (ImGui::DragFloat("##friction", &current_friction, 0.01f, 0.00f, FLT_MAX)) { SetFriction(current_friction); }
		ImGui::Title("Angular Fric.", 2);	if (ImGui::DragFloat("##angular_friction", &current_angular_friction, 0.01f, 0.00f, FLT_MAX)) { SetAngularFriction(current_angular_friction); }
		ImGui::Spacing();

	}

	ImGui::PopID();

	return true;
}

void ComponentCollider::DrawLayersCombo()
{
	ImGui::Title("Layer");

	if (ImGui::BeginComboEx(std::string("##layers").c_str(), std::string(" " + physics->layers->names[layer_num]).c_str(), 200, ImGuiComboFlags_NoArrowButton))
	{
		for (int n = 0; n < physics->layers->names.size(); ++n)
		{
			bool is_selected = (layer_num == n);

			if (ImGui::Selectable(std::string("   " + physics->layers->names[n]).c_str(), is_selected))
			{
				SetCollisionLayer(physics->layers->names[n]);
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}


void ComponentCollider::HandleAlienEvent(const AlienEvent& e)
{
	switch (e.type)
	{
	case AlienEventType::SCALE_CHANGED: {
		ScaleChanged();
		break;} 
	case AlienEventType::COLLISION_LAYER_STATE_CHANGED: {

		break; }
	case AlienEventType::COLLISION_LAYER_REMOVED: {

		break; }
	}
}

void ComponentCollider::InitCollider()
{
	shape->userData = this;
	SetCollisionLayer("Default");
}

void ComponentCollider::BeginUpdateShape()
{
	physics->DettachColldier(this, true);
}

void ComponentCollider::EndUpdateShape()
{
	physics->AttachCollider(this, true);
}