#include "Application.h"
#include "ComponentCollider.h"
#include "ComponentPointConstraint.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "GameObject.h"
#include "ReturnZ.h"

ComponentPointConstraint::ComponentPointConstraint(GameObject* go) : Component(go)
{
	// GameObject Components 
	type = ComponentType::POINT_CONSTRAINT;
	transform = game_object_attached->GetComponent<ComponentTransform>();

#ifndef GAME_VERSION
	App->objects->debug_draw_list.emplace(this, std::bind(&ComponentPointConstraint::DrawScene, this));
#endif // !GAME_VERSION
}

ComponentPointConstraint::~ComponentPointConstraint()
{
#ifndef GAME_VERSION
	App->objects->debug_draw_list.erase(App->objects->debug_draw_list.find(this));
#endif // !GAME_VERSION
}

void ComponentPointConstraint::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);

	if (connected_body)
		to_save->SetNumber("ConnectedBody", connected_body->ID);

	to_save->SetFloat3("PivotA", pivotA);
	to_save->SetFloat3("PivotB", pivotB);
	to_save->SetBoolean("BodiesCollision", disable_collision);
}

void ComponentPointConstraint::LoadComponent(JSONArraypack* to_load)
{

}

void ComponentPointConstraint::Update()
{

}

void ComponentPointConstraint::DrawScene()
{


}

void ComponentPointConstraint::Reset()
{
}

bool ComponentPointConstraint::DrawInspector()
{
	if (ImGui::CollapsingHeader(" Point Constraint", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
	}

	return true;
}

void ComponentPointConstraint::RecreateConstraint()
{
	/*if (constraint)
	{
		App->physics->RemoveConstraint(constraint);
		delete constraint;
		constraint = nullptr;
	}*/

	//ComponentRigidBody* c_rb_a = linked_go->GetComponent<ComponentRigidBody>();
}