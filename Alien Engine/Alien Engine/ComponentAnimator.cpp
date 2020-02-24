#include "ComponentAnimator.h"
#include "Application.h"
#include "ModuleResources.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "NodeEditor/Include/imgui_node_editor.h"
#include "ResourceAnimation.h"
#include "ResourceAnimatorController.h"
#include "ComponentAnimator.h"

ComponentAnimator::ComponentAnimator(GameObject* gameobject) : Component(gameobject)
{
	type = ComponentType::ANIMATOR;
	animator_controller = nullptr;
	animator_controller = new ResourceAnimatorController();
}

ComponentAnimator::~ComponentAnimator()
{
	//if (animator_controller)
	//	animator_controller->DecreaseReferences();
}

void ComponentAnimator::Update()
{
	if (!animator_controller)
		return;

	animator_controller->Update();

	if (game_object_attached && Time::state == Time::GameState::PLAY)
		UpdateAnimation(game_object_attached);
}

void ComponentAnimator::PlayState(std::string name)
{
	if (animator_controller)
		animator_controller->Play(name);
}

void ComponentAnimator::UpdateAnimation(GameObject* go_to_update)
{
	float3 position, scale;
	Quat rotation;

	if (animator_controller->GetTransform(go_to_update->GetName(), position, rotation, scale))
	{
		go_to_update->transform->SetLocalPosition(position);
		go_to_update->transform->SetLocalRotation(rotation);
		go_to_update->transform->SetLocalScale(scale);
	}

	for (int i = 0; i < go_to_update->GetChildren().size(); i++)
	{
		UpdateAnimation(go_to_update->GetChildren()[i]);
	}
}

void ComponentAnimator::Awake()
{
	if (animator_controller)
		animator_controller->Play();
}

ResourceAnimatorController* ComponentAnimator::GetResourceAnimatorController()
{
	return animator_controller;
}

void ComponentAnimator::SetAnimatorController(ResourceAnimatorController* controller)
{
	if (animator_controller)
		animator_controller->DecreaseReferences();

	animator_controller = controller;
	animator_controller->IncreaseReferences();
}

void ComponentAnimator::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	//more stuff
	to_save->SetBoolean("Enabled", enabled);
}

void ComponentAnimator::LoadComponent(JSONArraypack* to_load)
{
}

bool ComponentAnimator::DrawInspector()
{
	if (ImGui::CollapsingHeader("Animator")) {

	}

	return false;
}

//bool ComponentAnimator::Save(const nlohmann::json::iterator& it)
//{
//	nlohmann::json object = {
//		{ "active", active },
//		{ "type", type },
//		{ "controller", animator_controller ? animator_controller->GetID() : 0 }
//	};
//
//	it.value().push_back(object);
//
//	return true;
//}
//
//bool ComponentAnimator::Load(const nlohmann::json comp)
//{
//	active = comp["active"];
//	type = comp["type"];
//	uint c_id = comp["controller"];
//	if (c_id != 0)
//		animator_controller = (ResourceAnimatorController*)App->resources->GetAndReference(c_id);
//
//	return true;
//}