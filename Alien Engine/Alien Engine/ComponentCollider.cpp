#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"

ComponentCollider::ComponentCollider(GameObject* go) : Component(go)
{
	// Default values ---------------------------

	bouncing = 0.1f;
	friction = 0.5f;
	angular_friction = 0.1f;

	center = float3(0.f, 0.f, 0.f);

	// Create aux body --------------------------

	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.F, nullptr, nullptr);
	aux_body = new btRigidBody(rbInfo);
	aux_body->setUserPointer(go);
}

bool ComponentCollider::CleanUp()
{
	for (int i = 0; i < aux_body->getNumConstraintRefs(); ++i)
		aux_body->removeConstraintRef(aux_body->getConstraintRef(i));

	App->physics->RemoveBody(aux_body);

	delete shape;
	delete aux_motion_state;
	delete aux_body;

	return true;
}

void ComponentCollider::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("Bouncing", bouncing);
	to_save->SetNumber("Friction", friction);
	to_save->SetNumber("AngularFriction", angular_friction);
	to_save->SetFloat3("Center", center);
	to_save->SetBoolean("IsTrigger", is_trigger);

	SaveCollider(to_save);

}

void ComponentCollider::LoadComponent(JSONArraypack* to_load)
{
	//is_loaded = true;

	bouncing =			to_load->GetNumber("Bouncing");
	friction =			to_load->GetNumber("Friction");
	angular_friction =	to_load->GetNumber("AngularFriction");
	center =			to_load->GetFloat3("Center");
	is_trigger =		to_load->GetBoolean("IsTrigger");

	SetIsTrigger(is_trigger);
	LoadCollider(to_load);
}

void ComponentCollider::Update()
{
	// Load Collider on pdate -------------------------------

	if (shape == nullptr)
	{
		ComponentMesh* mesh = game_object_attached->GetComponent<ComponentMesh>();
		
		CreateShape(mesh);
		aux_body->setCollisionShape(shape);
	}

	// Match Size Scalling ----------------------------------

	AdjustShape();

	// Body Logic -------------------------------------------

	float3 world_center = GetWorldCenter();
	btTransform transform = ToBtTransform(linked_go->transform->GetPosition() + world_center, linked_go->transform->GetQuatRotation());
	aux_body->setWorldTransform(transform);

	if (App->time->GetGameState() == GameState::STOP) return true;

	if (SearchRigidBody())
	{
		if (body_added == true)
		{
			App->physics->RemoveBody(aux_body);
			body_added = false;
		}
	}
	else
	{
		if (body_added == false)
		{
			App->physics->AddBody(aux_body);
			body_added = true;
		}

		SetBouncing(bouncing);
		SetFriction(friction);
		SetAngularFriction(angular_friction);
	}

	return true;
}

bool ComponentCollider::Render()
{
	if (shape == nullptr) return true;

	if (App->scene->selected_go == linked_go && App->scene->editor_mode)
	{
		App->physics->RenderCollider(this);
	}

	return true;
}

void ComponentCollider::DrawPanelInfo()
{
	bool last_is_trigger = is_trigger;

	ImGui::Spacing();

	ImGui::Title("Is Trigger", 1);		ImGui::Checkbox("##is_trigger", &last_is_trigger);
	ImGui::Title("Center", 1);			ImGui::DragFloat3("##center", center.ptr(), 0.1f);

	// Draw Collider Adiional Info ---------------

	DrawPanelColliderInfo();


	ImGui::Spacing();
	ImGui::Title("Physic Material", 1); ImGui::Text("");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Title("Bouncing", 2);		ImGui::DragFloat("##bouncing", &bouncing, 0.01f, 0.00f, 1.f);
	ImGui::Title("Linear Fric.", 2);	ImGui::DragFloat("##friction", &friction, 0.01f, 0.00f, FLT_MAX);
	ImGui::Title("Angular Fric.", 2);	ImGui::DragFloat("##angular_friction", &angular_friction, 0.01f, 0.00f, FLT_MAX);

	ImGui::Spacing();

	if (last_is_trigger != is_trigger)
	{
		SetIsTrigger(last_is_trigger);
	}
}

void ComponentCollider::SetIsTrigger(bool value)
{
	if (value == true)
	{
		aux_body->setCollisionFlags(aux_body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	else
	{
		aux_body->setCollisionFlags(aux_body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}

	is_trigger = value;
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
	return linked_go->transform->GetGlobalTransform().RotatePart().MulPos(scaled_center);
}

bool ComponentCollider::SearchRigidBody()
{
	return (linked_go->GetComponent<C_RigidBody>() == nullptr) ? false : true;
}
