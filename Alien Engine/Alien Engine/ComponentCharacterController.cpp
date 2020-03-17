#include "Application.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"
#include <BulletDynamics\Character\btKinematicCharacterController.h>
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCharacterController.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ReturnZ.h"

#include "Time.h"
#include "ModuleInput.h"

ComponentCharacterController::ComponentCharacterController(GameObject* go) : Component(go)
{
	type = ComponentType::CHARACTER_CONTROLLER;
	// GameObject Components 
	transform =  game_object_attached->GetComponent<ComponentTransform>();
	shape = new btCapsuleShape(0.5f, 1);

	body = new btPairCachingGhostObject();
	body->setCollisionShape(shape);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);
	body->setWorldTransform(ToBtTransform(transform->GetGlobalPosition(), transform->GetGlobalRotation()));

	controller = new btKinematicCharacterController(body, (btConvexShape*)shape, 0.5);
	controller->setUp(btVector3(0.f, 1.f, 0.f));
	controller->setGravity(btVector3(0.f, -10.f, 0.f));

	max_jump_height = 10.f;
	jump_speed = controller->getJumpSpeed();
	fall_speed = controller->getFallSpeed();

	App->physics->world->addCollisionObject(body, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::CharacterFilter | btBroadphaseProxy::DefaultFilter);
	App->physics->AddAction(controller);
}

ComponentCharacterController::~ComponentCharacterController()
{
	App->physics->RemoveAction(controller);
	App->physics->world->removeCollisionObject(body);

	delete shape;
}

// Movement Functions -----------------------------------------

void ComponentCharacterController::SetWalkDirection(float3 direction)
{
	controller->setWalkDirection(ToBtVector3(direction));
}

void ComponentCharacterController::SetMaxJumpHeight(const float max_jump_height)
{
	this->max_jump_height = max_jump_height;
	controller->setMaxJumpHeight(max_jump_height);
}

void ComponentCharacterController::SetJumpSpeed(const float jump_speed)
{
	this->jump_speed = jump_speed;
	controller->setJumpSpeed(jump_speed);
}

void ComponentCharacterController::SetFallSpeed(const float fall_speed)
{
	this->fall_speed = fall_speed;
	controller->setFallSpeed(fall_speed);
}

void ComponentCharacterController::ApplyImpulse(float3 direction)
{
	controller->applyImpulse({ direction.x, direction.y, direction.z });
}

void ComponentCharacterController::Jump(float3 direction)
{
	controller->jump(ToBtVector3(direction));
}

bool ComponentCharacterController::CanJump()
{
	return controller->canJump();
}

void ComponentCharacterController::SetRotation(const Quat rotation)
{
	body->setWorldTransform(ToBtTransform(transform->GetGlobalPosition(), rotation));
	transform->SetGlobalRotation(math::Quat(rotation));
}
// -------------------------------------------------------------

void ComponentCharacterController::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("MaxJumpHeight", max_jump_height);
	to_save->SetNumber("JumpSpeed", jump_speed);
	to_save->SetNumber("FallSpeed", fall_speed);
}

void ComponentCharacterController::LoadComponent(JSONArraypack* to_load)
{
	SetMaxJumpHeight(to_load->GetNumber("MaxJumpHeight"));
	SetJumpSpeed(to_load->GetNumber("JumpSpeed"));
	SetFallSpeed(to_load->GetNumber("FallSpeed"));
}

bool ComponentCharacterController::DrawInspector()
{
	float current_max_jump_height = max_jump_height;
	float current_jump_speed = jump_speed;
	float current_fall_speed = fall_speed;

	if (ImGui::CollapsingHeader(" Character Controller", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();
		ImGui::Title("Max Jump Height", 1);		if (ImGui::DragFloat("##max_jump_height", &current_max_jump_height, 0.01f, 0.00f, FLT_MAX)) { SetMaxJumpHeight(current_max_jump_height); }
		ImGui::Title("Jump Speed", 1);			if (ImGui::DragFloat("##jump_speed", &current_jump_speed, 0.01f, 0.00f, FLT_MAX))			{ SetJumpSpeed(current_jump_speed); }
		ImGui::Title("Fall Speed", 1);			if (ImGui::DragFloat("##fall_speed", &current_fall_speed, 0.01f, 0.00f, FLT_MAX))			{ SetFallSpeed(current_fall_speed); }
		ImGui::Spacing();
	}
	return true;
}

void ComponentCharacterController::Update()
{
	if (Time::IsPlaying()) 
	{
		btTransform bt_transform = body->getWorldTransform();
		btQuaternion rotation = bt_transform.getRotation();
		btVector3 position = bt_transform.getOrigin();

		body->activate(true);
		transform->SetGlobalPosition(float3(position));
		transform->SetGlobalRotation(math::Quat(rotation));
	}
	else
	{
		body->setWorldTransform(ToBtTransform(transform->GetGlobalPosition(), transform->GetGlobalRotation()));
	}
}

void ComponentCharacterController::DrawScene()
{
	if (game_object_attached->IsSelected() && App->physics->debug_physics == false)
	{
		App->physics->DrawCharacterController(this);
	}
}

void ComponentCharacterController::Reset()
{
}



