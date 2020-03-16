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

// -------------------------------------------------------------

void ComponentCharacterController::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("Test", test);
}

void ComponentCharacterController::LoadComponent(JSONArraypack* to_load)
{
	test = to_load->GetBoolean("Test");
}

void ComponentCharacterController::Update()
{
	if (Time::IsPlaying()) 
	{
		//
		//float3 movement = float3::zero();
	
		//if (!test)
		//{
		//	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_REPEAT)
		//		movement.x -= 1;
		//	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_REPEAT)
		//		movement.x += 1;
		//	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_STATE::KEY_REPEAT)
		//		movement.z += 1;
		//	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_REPEAT)
		//		movement.z -= 1;
		//}
		//else
		//{
		//	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_STATE::KEY_REPEAT)
		//		movement.x -= 1;
		//	if (App->input->GetKey(SDL_SCANCODE_K) == KEY_STATE::KEY_REPEAT)
		//		movement.x += 1;
		//	if (App->input->GetKey(SDL_SCANCODE_J) == KEY_STATE::KEY_REPEAT)
		//		movement.z += 1;
		//	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_STATE::KEY_REPEAT)
		//		movement.z -= 1;
		//}

		//float speed = (movement.Equals(float3::zero())) ? 0.f : 4.f ;
		//SetWalkDirection(movement.Normalized() * speed * Time::GetDT());

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

bool ComponentCharacterController::DrawInspector()
{
	if (ImGui::CollapsingHeader(" Character Controller", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Title("Test", 1);	if (ImGui::Checkbox("##test", &test));
	}

	return true;
}

