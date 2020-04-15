#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "ComponentPhysics.h"
#include "ComponentScript.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ReturnZ.h"
#include "Alien.h"
#include "Time.h"

ComponentCollider::ComponentCollider(GameObject* go) : ComponentBasePhysic(go)
{
	std::vector<ComponentScript*> scripts = go->GetComponents<ComponentScript>();
	for (ComponentScript* script : scripts)
		if (script->need_alien == true)
			alien_scripts.push_back(script);

	// Default values 
	center = float3::zero();
	rotation = float3::zero();
}

ComponentCollider::~ComponentCollider()
{
	App->SendAlienEvent(this, AlienEventType::COLLIDER_DELETED);
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
	trans.q = QUAT_TO_PXQUAT( Quat::FromEulerXYZ(rad_rotation.x , rad_rotation.y, rad_rotation.z));
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
		shape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, true); }
	else {
		shape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, false);
		shape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, true); }
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
}

void ComponentCollider::Update()
{
	if (!alien_scripts.empty() && Time::IsPlaying())
	{
		for (auto& x : collisions)
		{
			x.second = false;
		}

		ComponentCollider* coll = nullptr;// TODO: Get the collider 
		std::map<ComponentCollider*, bool>::iterator search = collisions.find(coll);
		if (search != collisions.end() && coll != nullptr)
		{
			collisions[coll] = true;

			for (ComponentScript* script : alien_scripts)
			{
				Alien* alien = (Alien*)script->data_ptr;
				try {
					alien->OnTrigger(coll);
				}
				catch (...)
				{
					try {
						LOG_ENGINE("ERROR IN THE SCRIPT %s WHEN CALLING ON TRIGGER", alien->data_name);
					}
					catch (...) {
						LOG_ENGINE("UNKNOWN ERROR IN SCRIPT WHEN CALLING ON TRIGGER");
					}
				}
			}
		}
		else
		{
			collisions[coll] = true;

			for (ComponentScript* script : alien_scripts)
			{
				Alien* alien = (Alien*)script->data_ptr;
				try {
					alien->OnTriggerEnter(coll);
				}
				catch (...)
				{
					try {
						LOG_ENGINE("ERROR IN THE SCRIPT %s WHEN CALLING ON TRIGGER ENTER", alien->data_name);
					}
					catch (...) {
						LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS WHEN CALLING ON TRIGGER ENTER");
					}
				}
			}
		}

		std::map<ComponentCollider*, bool>::iterator itr = collisions.begin();

		while (itr != collisions.end())
		{
			if (itr->second == false) {

				for (ComponentScript* script : alien_scripts)
				{
					Alien* alien = (Alien*)script->data_ptr;
					try {
						alien->OnTriggerExit(itr->first);
					}
					catch (...)
					{
						try {
							LOG_ENGINE("ERROR IN THE SCRIPT %s WHEN CALLING ON TRIGGER EXIT", alien->data_name);
						}
						catch (...) {
							LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS WHEN CALLING ON TRIGGER EXIT");
						}
					}
				}

				itr = collisions.erase(itr);
			}
			else {
				++itr;
			}

		}
	}
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
	if (enabled == true && game_object_attached->IsSelected() && App->physics->debug_physics == false)
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

		ImGui::Title("Layer");

		if (ImGui::BeginComboEx(std::string("##layers").c_str(), std::string(" " + App->physics->layers.at(layer)).c_str(), 200, ImGuiComboFlags_NoArrowButton))
		{
			for (int n = 0; n < App->physics->layers.size(); ++n)
			{
				bool is_selected = (layer == n);

				if (ImGui::Selectable(std::string("   " + App->physics->layers.at(n)).c_str(), is_selected))
				{
					layer = n;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

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

void ComponentCollider::HandleAlienEvent(const AlienEvent& e)
{
	switch (e.type)
	{
	case AlienEventType::SCRIPT_ADDED: {
		ComponentScript* script = (ComponentScript*)e.object;
		if (script->game_object_attached == game_object_attached && script->need_alien == true)
			alien_scripts.push_back(script);
		break; }
	case AlienEventType::SCRIPT_DELETED: {
		ComponentScript* script = (ComponentScript*)e.object;
		if (script->game_object_attached == game_object_attached)
			alien_scripts.remove(script);
		break; }
	case AlienEventType::COLLIDER_DELETED: {
		ComponentCollider* collider = (ComponentCollider*)e.object;

		if (!alien_scripts.empty() && Time::IsPlaying())
		{
			if (!collisions.empty() && collisions.find(collider) != collisions.end())
			{
				for (ComponentScript* script : alien_scripts)
				{
					Alien* alien = (Alien*)script->data_ptr;
					alien->OnTriggerExit(collider);
				}
				collisions.erase(collider);
			}
		}
		break; }
	}
}

void ComponentCollider::BeginUpdateShape()
{
	physics->DettachColldier(this, true);
}

void ComponentCollider::EndUpdateShape()
{
	physics->AttachCollider(this, true);
}