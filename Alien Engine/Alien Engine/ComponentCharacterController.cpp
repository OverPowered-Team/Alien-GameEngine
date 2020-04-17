#include "Application.h"
#include "ModulePhysX.h"
#include "ModuleRenderer3D.h"
#include "ComponentCharacterController.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ReturnZ.h"

#include "UtilitiesPhysX.h"

#include "Time.h"
#include "ModuleInput.h"
#include "mmgr/mmgr.h"

ComponentCharacterController::ComponentCharacterController(GameObject* go) : ComponentCollider(go)
{
	type = ComponentType::CHARACTER_CONTROLLER;
	// GameObject Components 
	transform = go->GetComponent<ComponentTransform>();

	// load default settings
	SetDefaultConf();

	controller = App->physx->CreateCharacterController(desc);

	moveDirection = float3::zero();

	//controller->setUserData(this); // TODO: adapt component collider
}

ComponentCharacterController::~ComponentCharacterController()
{
	controller->release();
}

// Movement Functions -----------------------------------------

//void ComponentCharacterController::SetWalkDirection(float3 direction)
//{
//	controller->setWalkDirection(ToBtVector3(direction));
//}
//
//
//void ComponentCharacterController::SetJumpSpeed(const float jump_speed)
//{
//	this->jump_speed = jump_speed;
//	controller->setJumpSpeed(jump_speed);
//}
//
//void ComponentCharacterController::SetGravity(const float gravity)
//{
//	this->gravity = gravity;
//	controller->setGravity(ToBtVector3(float3(0.f, -gravity, 0.f)));
//}
//
//void ComponentCharacterController::ApplyImpulse(float3 direction)
//{
//	controller->applyImpulse({ direction.x, direction.y, direction.z });
//}
//
//void ComponentCharacterController::Jump(float3 direction)
//{
//	controller->jump(ToBtVector3(direction));
//}
//
//bool ComponentCharacterController::CanJump()
//{
//	return controller->canJump();
//}
//
//bool ComponentCharacterController::OnGround()
//{
//	return controller->onGround();
//}
//
//void ComponentCharacterController::SetRotation(const Quat rotation)
//{
//	body->setWorldTransform(ToBtTransform(transform->GetGlobalPosition() + character_offset, rotation));
//	transform->SetGlobalRotation(math::Quat(rotation));
//}
//
//Quat ComponentCharacterController::GetRotation() const
//{
//	return transform->GetGlobalRotation();
//}
//
//void ComponentCharacterController::SetPosition(const float3 pos)
//{
//	body->setWorldTransform(ToBtTransform(pos + character_offset, transform->GetGlobalRotation()));
//	transform->SetGlobalPosition(pos);
//}
//
//float3 ComponentCharacterController::GetPosition() const
//{
//	return transform->GetGlobalPosition();
//}
//
//void ComponentCharacterController::SetCharacterOffset(const float3 offset)
//{
//	character_offset = offset;
//}
//

void ComponentCharacterController::SetContactOffset(const float contactOffset)
{
	float max = FLT_MAX, min = 0.0001f;
	desc.contactOffset = Clamp(contactOffset, min, max);

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE:
	{
		PxCapsuleController* capsule_c = (PxCapsuleController*)controller;
		capsule_c->setContactOffset(desc.contactOffset);
		break;
	}
	default:
		break;
	}
}

void ComponentCharacterController::SetStepOffset(const float stepOffset)
{
	float max = 180.0f, min = 0.0f;
	desc.slopeLimit = Clamp(stepOffset, min, max);

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE:
	{
		PxCapsuleController* capsule_c = (PxCapsuleController*)controller;
		capsule_c->setSlopeLimit(desc.slopeLimit);
		break;
	}
	default:
		break;
	}
}

void ComponentCharacterController::SetSlopeLimit(const float slopeLimit)
{
	float max = 180.0f, min = 0.0f;
	desc.slopeLimit = Clamp(slopeLimit, min, max);

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE:
	{
		PxCapsuleController* capsule_c = (PxCapsuleController*)controller;
		capsule_c->setSlopeLimit(cosf(DegToRad(desc.slopeLimit)));
		break;
	}
	default:
		break;
	}
}

void ComponentCharacterController::SetCharacterHeight(const float height)
{
	float max = FLOAT_INF, min = 0.0f; // 0 means sphere
	desc.height = Clamp(height, min, max);

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE:
	{
		PxCapsuleController* capsule_c = (PxCapsuleController*)controller;
		capsule_c->setHeight(desc.height);
		break;
	}
	default:
		break;
	}

	//RecreateCapusle();
}

void ComponentCharacterController::SetCharacterRadius(const float radius)
{
	float max = FLOAT_INF, min = 0.1f;
	desc.radius = Clamp(radius, min, max);

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE:
	{
		PxCapsuleController* capsule_c = (PxCapsuleController*)controller;
		capsule_c->setRadius(desc.radius);
		break;
	}
	default:
		break;
	}

	//RecreateCapusle();
}
//// -------------------------------------------------------------
//
//void ComponentCharacterController::RecreateCapusle()
//{
//	shape->setLocalScaling(btVector3(character_radius * 2, character_height, character_radius * 2));
//}

void ComponentCharacterController::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);

	//to_save->SetFloat3("CharacterOffset", character_offset);
	to_save->SetNumber("CharacterRadius", desc.radius);
	to_save->SetNumber("CharacterHeight", desc.height);
	//to_save->SetNumber("JumpSpeed", jump_speed);
	//to_save->SetNumber("Gravity", gravity);
}

void ComponentCharacterController::LoadComponent(JSONArraypack* to_load)
{
	//SetCharacterOffset(to_load->GetFloat3("CharacterOffset"));
	SetCharacterRadius(to_load->GetNumber("CharacterRadius"));
	SetCharacterHeight(to_load->GetNumber("CharacterHeight"));
	//SetJumpSpeed(to_load->GetNumber("JumpSpeed"));
	//SetGravity(to_load->GetNumber("Gravity"));
}

bool ComponentCharacterController::DrawInspector()
{
	/*float3 current_character_offset = character_offset;
	float current_character_radius = character_radius;
	float current_character_height = character_height;
	float current_jump_speed = jump_speed;
	float current_gravity = gravity;*/

	ImGui::PushID(this);

	if (ImGui::CollapsingHeader(" Character Controller", &not_destroy))
	{
		ImGui::Spacing();

		//ImGui::Title("Layer");

		/*if (ImGui::BeginComboEx(std::string("##layers").c_str(), std::string(" " + App->physics->layers.at(collider->layer)).c_str(), 200, ImGuiComboFlags_NoArrowButton))
		{
			for (int n = 0; n < App->physics->layers.size(); ++n)
			{
				bool is_selected = (collider->layer == n);

				if (ImGui::Selectable(std::string("   " + App->physics->layers.at(n)).c_str(), is_selected))
				{
					collider->layer = n;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}*/
		//ImGui::Title("Offset", 1);				if (ImGui::DragFloat3("##center", current_character_offset.ptr(), 0.05f)) { SetCharacterOffset(current_character_offset); }
		ImGui::Title("Slope Limit", 1);				if (ImGui::DragFloat("##slopeLimit", &desc.slopeLimit, 0.03f, 0.0f, 180.0f))		{ SetSlopeLimit(desc.slopeLimit); }
		ImGui::Title("Step Offset", 1);				if (ImGui::DragFloat("##stepOffset", &desc.stepOffset, 0.03f, 0.0f, FLT_MAX))		{ SetStepOffset(desc.stepOffset); }
		ImGui::Title("Skin Width", 1);				if (ImGui::DragFloat("##skinWidth", &desc.contactOffset, 0.03f, 0.0001f, FLT_MAX))	{ SetContactOffset(desc.contactOffset); }
		ImGui::Title("Min Move Distance", 1);	ImGui::DragFloat("##minDist", &min_distance, 0.03f, 0.f, FLT_MAX);

		ImGui::Title("Radius", 1);					if (ImGui::DragFloat("##radius", &desc.radius, 0.05f, 0.1f, FLT_MAX))				{ SetCharacterRadius(desc.radius); }
		ImGui::Title("Height", 1);					if (ImGui::DragFloat("##height", &desc.height, 0.05f, 0.0f, FLT_MAX))				{ SetCharacterHeight(desc.height); }
		
		ImGui::Separator();
		ImGui::Title("Experimental features");
		ImGui::Separator();

		ImGui::Title("Gravity", 1);					ImGui::DragFloat("##gravity", &gravity, 0.01f, 0.00f, FLT_MAX);
		ImGui::Title("Force gravity", 1);			ImGui::Checkbox("##forceGravity", &force_gravity);
		ImGui::Title("Force move", 1);				ImGui::Checkbox("##forceMove", &force_move);
		//ImGui::Title("Jump Speed", 1);			if (ImGui::DragFloat("##jump_speed", &current_jump_speed, 0.01f, 0.00f, FLT_MAX)) { SetJumpSpeed(current_jump_speed); }
		
		
		//ImGui::Checkbox("isGrounded", &isGrounded); // debug test

		ImGui::Separator();
		ImGui::Text("velocity: %f,%f,%f", velocity.x, velocity.y, velocity.z);
		ImGui::Spacing();
	}

	ImGui::PopID();

	return true;
}

void ComponentCharacterController::Update()
{
	//collider->Update();

	if (Time::IsPlaying())
	{
		// TODO DELETE hardcoded test 1 -----------------------------------------------------------------------
		//moveDirection.x = moveDirection.z = 0.0f;
		//float speed = 10.0f;
		//if (App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_REPEAT)
		//	moveDirection.x -= 1.0f;
		//if (App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_REPEAT)
		//	moveDirection.x += 1;
		//if (App->input->GetKey(SDL_SCANCODE_S) == KEY_STATE::KEY_REPEAT)
		//	moveDirection.z += 1;
		//if (App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_REPEAT)
		//	moveDirection.z -= 1;
		//if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_STATE::KEY_DOWN)
		//	moveDirection.y = 8.0f;

		//moveDirection = float3(moveDirection.x * speed, moveDirection.y, moveDirection.z * speed) ; // SPEED
		// ----------------------------------------------------------------------------------------------------

		if (force_move && isGrounded && velocity.isZero())
		{
			// always force downwards to mantain collision state 
			// (cct solver puts on top of skin and not returns collision state when performs a 0y)
			Move(float3(0.0f, -controller->getContactOffset(), 0.0f));
		}

		if (force_gravity && !isGrounded)
		{
			moveDirection.y -= gravity * Time::GetDT();
			Move(moveDirection * Time::GetDT());
			isGrounded ? moveDirection.y = 0.0f : 0;
		}
		//else // TODO: DELETE HARDCODED TEST from 1
		//{
		//	Move(float3(moveDirection.x, moveDirection.y - controller->getContactOffset(), moveDirection.z) * Time::GetDT());
		//}

		transform->SetGlobalPosition(PXVEC3EXT_TO_F3(controller->getPosition()));
	}
	else
	{
		controller->setPosition(F3_TO_PXVEC3EXT(transform->GetGlobalPosition()));
	}
}

PxControllerCollisionFlags ComponentCharacterController::Move(float3 motion)
{

	// set velocity on current position before move
	velocity = controller->getPosition();

	// perform the move
	PxControllerFilters filters; // TODO: implement filters callback when needed
	collisionFlags = controller->move(F3_TO_PXVEC3(motion), min_distance, Time::GetDT(), filters);

	// set grounded internal state
	collisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN) ? isGrounded = true : isGrounded = false;

	// substract the difference from current pos to velocity before move
	velocity = PXVEC3_TO_VEC3EXT(controller->getPosition() - velocity);//isGrounded ? F3_TO_PXVEC3EXT(float3::zero()) : PXVEC3_TO_VEC3EXT(controller->getPosition() - velocity);

	return collisionFlags;
}

void ComponentCharacterController::DrawScene()
{
	if (game_object_attached->IsSelected() && App->physics->debug_physics == false)
	{
		switch (desc.getType())
		{
		case PxControllerShapeType::eCAPSULE:
		{
			PxCapsuleController* capsule = (PxCapsuleController*)controller;
			float skin_offset = capsule->getContactOffset();

			//float4x4 transform = PXTRANS_TO_F4X4(capsule->getActor()->getGlobalPose());

			float radius = capsule->getRadius() + skin_offset;
			float height = capsule->getHeight() + skin_offset; //


			float3 oriblue(0.15f, 0.3f, 0.95f);
			PxExtendedVec3 pos = capsule->getPosition();
			PxTransform trans;
			trans.p = PxVec3(pos.x, pos.y, pos.z);
			trans.q = QUAT_TO_PXQUAT(Quat::FromEulerXYZ(0.0f, 0.0f, DEGTORAD * 90.0f));
			App->renderer3D->DebugDrawCapsule(PXTRANS_TO_F4X4(trans), radius, height * 0.5f, oriblue);

			break;
		}
		case PxControllerShapeType::eBOX:
		{
			break;
		}
		default:
			break;
		}
	}

}



void ComponentCharacterController::HandleAlienEvent(const AlienEvent& e)
{
	//collider->HandleAlienEvent(e);
}

void ComponentCharacterController::SetDefaultConf()
{
	desc.radius = 1.0f;
	desc.height = 2.0f;
	desc.material = App->physx->CreateMaterial(static_friction, dynamic_friction, restitution);
	desc.position = F3_TO_PXVEC3EXT(game_object_attached->transform->GetGlobalPosition());
	desc.slopeLimit = cosf(DegToRad(45.0f));
	//desc.upDirection = // up direction can be changed to simulate planetoids or surfaces with other gravity dir vector

	min_distance = 0.001f;
}