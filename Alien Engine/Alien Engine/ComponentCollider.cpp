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
	collider(collider), rigid_body(rigid_body), transform(transform), contancts(contancts), num_contact(num_contact), game_object(game_object), impulse(impulse), relative_velocity(relative_velocity) {}

ComponentCollider::ComponentCollider(GameObject* go) : ComponentBasePhysic(go)
{
	// Default values 
	center = float3::zero();
	rotation = float3::zero();
}

ComponentCollider::~ComponentCollider()
{
	if (!IsController()) {
		go->SendAlientEventThis(this, AlienEventType::COLLIDER_DELETED);
		shape->release();
		shape = nullptr;
	}

}

// Colliders Functions --------------------------------

void ComponentCollider::SetCenter(const float3& value)
{
	center = value;
	PxTransform trans = shape->getLocalPose();
	trans.p = F3_TO_PXVEC3(center.Mul(physics->scale));
	BeginUpdateShape();
	shape->setLocalPose(trans);
	EndUpdateShape();
}

void ComponentCollider::SetRotation(const float3& value)
{
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
	shape->setQueryFilterData(filter_data);
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
	if (!IsController())
		go->SendAlientEventThis(this, AlienEventType::COLLIDER_ENABLED);
	else
		go->SendAlientEventThis(this, AlienEventType::CHARACTER_CTRL_ENABLED);
}

void ComponentCollider::OnDisable()
{
	if (!IsController())
		go->SendAlientEventThis(this, AlienEventType::COLLIDER_DISABLED);
	else
		go->SendAlientEventThis(this, AlienEventType::CHARACTER_CTRL_DISABLED);
}

void ComponentCollider::DrawScene()
{
	if (enabled == true && (game_object_attached->IsSelected() || App->physx->debug_physics))
	{
		App->physx->DrawCollider(this);
	}
}

void ComponentCollider::Reset()
{
	BeginUpdateShape(true);
	SetCenter(float3::zero());
	SetRotation(float3::zero());
	SetIsTrigger(false);
	SetBouncing(0.1f);
	SetFriction(0.5f);
	SetAngularFriction(0.1f);
	EndUpdateShape(true);
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

		//ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() * 0.5f - 30);
		ImGui::SetCursorPosX(12.0f);
		if (ImGui::Button("fit", ImVec2(60.0f, 22.0f)))
		{
			Reset();
		}

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
	case AlienEventType::PHYSICS_SCALE_CHANGED: {
		ScaleChanged();
		break; }
	case AlienEventType::COLLISION_LAYER_STATE_CHANGED: {

		break; }
	case AlienEventType::COLLISION_LAYER_REMOVED: {

		break; }
	}
}

void ComponentCollider::InitCollider()
{
	shape->userData = this;
	go->SendAlientEventThis(this, AlienEventType::COLLIDER_ADDED);
	SetCollisionLayer("Default");
}

void ComponentCollider::BeginUpdateShape(bool force_update)
{
	if (!this->force_update)
		physics->DettachCollider(this, true);

	if (force_update)
		this->force_update = true;
}

void ComponentCollider::EndUpdateShape(bool force_update)
{
	if (force_update)
		this->force_update = false;

	if (!this->force_update)
		physics->AttachCollider(this, true);
}
const float3 ComponentCollider::GetLocalMeshAabbSize() const
{
	const ComponentMesh* mesh = GetMesh();
	if (!mesh)
		return float3::one();

	return mesh->GetLocalAABB().Size();
}

const AABB ComponentCollider::GetLocalMeshAabb() const
{
	const ComponentMesh* mesh = GetMesh();
	if (!mesh)
		AABB(Sphere(float3::zero(), 0.5f));

	return mesh->GetLocalAABB();
}

const ComponentMesh* ComponentCollider::GetMesh() const
{
	return game_object_attached->GetComponent<ComponentMesh>();
}