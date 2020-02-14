#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "ResourceBone.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBone.h"



ComponentBone::ComponentBone(GameObject* gameobject) : Component(gameobject)
{
}


ComponentBone::~ComponentBone()
{
}

void ComponentBone::Draw()
{
	if (!debug_draw)
		return;

	//App->renderer3D->DrawSphere(gameobject->transform->GetPosition(), gameobject->transform->GetScale().x * 0.5f);
}

void ComponentBone::AddBone(ResourceBone* b)
{
	if (b)
		bone = b;
}

ResourceBone* ComponentBone::GetBone()
{
	return bone;
}

//bool ComponentBone::Save(const nlohmann::json::iterator& it)
//{
//	nlohmann::json object = {
//		{ "active", active },
//		{ "type", type },
//		{ "resource", bone->GetID() },
//		{ "debug_draw", debug_draw },
//	};
//
//	it.value().push_back(object);
//	return true;
//}
//
//bool ComponentBone::Load(const nlohmann::json comp)
//{
//	active = comp["active"];
//	type = comp["type"];
//	bone = (ResourceBone*)App->resources->GetAndReference(comp["resource"]);
//	debug_draw = comp["debug_draw"];
//
//	return true;
//}
