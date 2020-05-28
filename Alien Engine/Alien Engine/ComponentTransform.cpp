#include "ComponentTransform.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "ModuleObjects.h"
#include "Application.h"
#include "ComponentScript.h"
#include "ReturnZ.h"
#include "ComponentMesh.h"
#include "PanelScene.h"
#include "ResourcePrefab.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "ModuleResources.h"
#include "PanelProject.h"
#include "ComponentCurve.h"
#include "Alien.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

ComponentTransform::ComponentTransform(GameObject* attach) : Component(attach)
{
	type = ComponentType::TRANSFORM;	
}

ComponentTransform::ComponentTransform(GameObject* attach, const float3& pos, const Quat& rot, const float3& scale) : Component(attach)
{
	local_position = pos;
	local_rotation = rot;
	local_scale = scale;
	euler_rotation = local_rotation.ToEulerXYZ();
	euler_rotation.x = RadToDeg(euler_rotation.x);
	euler_rotation.y = RadToDeg(euler_rotation.y);
	euler_rotation.z = RadToDeg(euler_rotation.z);
	local_transformation = float4x4::FromTRS(local_position, local_rotation, local_scale);

	if (game_object_attached->parent != nullptr) {
		ComponentTransform* tr = (ComponentTransform*)game_object_attached->parent->GetComponent(ComponentType::TRANSFORM);
		if (tr != nullptr) global_transformation = tr->global_transformation * local_transformation;
		else global_transformation = local_transformation;
	}
	else
		global_transformation = local_transformation;

	up = { 2 * (local_rotation.x * local_rotation.y - local_rotation.w * local_rotation.z),
			1 - 2 * (local_rotation.x * local_rotation.x + local_rotation.z * local_rotation.z),
			2 * (local_rotation.y * local_rotation.z + local_rotation.w * local_rotation.x) };
	forward = { 2 * (local_rotation.x * local_rotation.z + local_rotation.w * local_rotation.y),
			2 * (local_rotation.y * local_rotation.z - local_rotation.w * local_rotation.x),
			1 - 2 * (local_rotation.x * local_rotation.x + local_rotation.y * local_rotation.y) };
	right = { 1 - 2 * (local_rotation.y * local_rotation.y + local_rotation.z * local_rotation.z),
		 2 * (local_rotation.x * local_rotation.y + local_rotation.w * local_rotation.z),
		 2 * (local_rotation.x * local_rotation.z + local_rotation.w * local_rotation.y) };

	type = ComponentType::TRANSFORM;
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::SetLocalPosition(const float3& new_local_pos)
{
	OPTICK_EVENT();
	local_position = new_local_pos;

	RecalculateTransform();
}

void ComponentTransform::SetLocalPosition(const float& x, const float& y, const float& z)
{
	OPTICK_EVENT();
	local_position.x = x;
	local_position.y = y;
	local_position.z = z;

	RecalculateTransform();
}

void ComponentTransform::SetGlobalPosition(const float3& pos)
{
	OPTICK_EVENT();
	global_transformation.SetTranslatePart(pos);
	GameObject* parent = game_object_attached->parent;


	if (parent != nullptr)
	{
		ComponentTransform* paremt_tramsform = parent->GetComponent<ComponentTransform>();

		if (paremt_tramsform != nullptr)
		{
			local_transformation = paremt_tramsform->global_transformation.Inverted() * global_transformation;
		}
		else
		{
			local_transformation = global_transformation;
		}
	}
	else
	{
		local_transformation = global_transformation;
	}

	local_transformation.Decompose(local_position, local_rotation, local_scale);
	RecalculateTransform();
}

void ComponentTransform::SetGlobalRotation(Quat rotation)
{
	OPTICK_EVENT();
	global_transformation = float4x4::FromTRS(global_transformation.TranslatePart(), rotation, global_transformation.GetScale());
	GameObject* parent = game_object_attached->parent;


	if (parent != nullptr)
	{
		ComponentTransform* paremt_tramsform = parent->GetComponent<ComponentTransform>();

		if (paremt_tramsform != nullptr)
		{
			local_transformation = paremt_tramsform->global_transformation.Inverted() * global_transformation;
		}
		else
		{
			local_transformation = global_transformation;
		}
	}
	else
	{
		local_transformation = global_transformation;
	}

	local_transformation.Decompose(local_position, local_rotation, local_scale);
	RecalculateTransform();

}

const float3 ComponentTransform::GetLocalPosition() const
{
	return local_position;
}

const float3 ComponentTransform::GetGlobalPosition() const
{
	return global_transformation.Float3x4Part().Col(3);
}

void ComponentTransform::SetLocalScale(const float3& new_local_scale)
{
	OPTICK_EVENT();
	local_scale = new_local_scale;

	LookScale();

	RecalculateTransform();
}

void ComponentTransform::SetLocalScale(const float& x, const float& y, const float& z)
{
	OPTICK_EVENT();
	local_scale.x = x;
	local_scale.y = y;
	local_scale.z = z;

	LookScale();

	RecalculateTransform();
}

void ComponentTransform::SetLocalTransform(const float3& position, const Quat& rotation, const float3& scale)
{
	OPTICK_EVENT();
	local_position = position;
	local_rotation = rotation;
	local_scale = scale;

	RecalculateTransform();
}

void ComponentTransform::LookScale()
{
	bool aux_scale = is_scale_negative;

	if (local_scale.x < 0 || local_scale.y < 0 || local_scale.z < 0) {
		is_scale_negative = true;
	}
	else is_scale_negative = false;

	if (aux_scale != is_scale_negative)
		game_object_attached->ScaleNegative(is_scale_negative);
}

const float3 ComponentTransform::GetLocalScale() const
{
	return local_scale;
}

const float3 ComponentTransform::GetGlobalScale() const
{
	float3 scale;
	float3x3 rotate;
	rotate = global_transformation.RotatePart();
	scale.x = rotate.Col(0).Length();
	scale.y = rotate.Col(1).Length();
	scale.z = rotate.Col(2).Length();
	return scale;
}

void ComponentTransform::SetLocalRotation(const Quat& new_local_rotation)
{
	OPTICK_EVENT();
	local_rotation.Set(new_local_rotation.x, new_local_rotation.y, new_local_rotation.z, new_local_rotation.w);

	euler_rotation = local_rotation.ToEulerXYZ();
	euler_rotation.x = RadToDeg(euler_rotation.x);
	euler_rotation.y = RadToDeg(euler_rotation.y);
	euler_rotation.z = RadToDeg(euler_rotation.z);

	RecalculateTransform();
}

void ComponentTransform::SetLocalRotation(const float& x, const float& y, const float& z, const float& angle)
{
	OPTICK_EVENT();
	local_rotation.x = x;
	local_rotation.y = y;
	local_rotation.z = z;
	local_rotation.w = angle;

	euler_rotation = local_rotation.ToEulerXYZ();
	euler_rotation.x = RadToDeg(euler_rotation.x);
	euler_rotation.y = RadToDeg(euler_rotation.y);
	euler_rotation.z = RadToDeg(euler_rotation.z);

	RecalculateTransform();
}



const Quat ComponentTransform::GetLocalRotation() const
{
	return local_rotation;
}

const Quat ComponentTransform::GetGlobalRotation() const
{
	float3 pos, scale;
	Quat rot;

	global_transformation.Decompose(pos, rot, scale);

	return rot;
}

float4x4 ComponentTransform::GetGlobalMatrix() const
{
	return global_transformation;
}

void ComponentTransform::RecalculateTransform()
{
	OPTICK_EVENT();
	if (game_object_attached == nullptr)
		return;

	float3 final_scale = float3(
		(local_scale.x != 0.f) ? local_scale.x : 0.001f,
		(local_scale.y != 0.f) ? local_scale.y : 0.001f,
		(local_scale.z != 0.f) ? local_scale.z : 0.001f);

	local_transformation = float4x4::FromTRS(local_position, local_rotation, final_scale);

	if (game_object_attached->parent != nullptr) 
	{
		ComponentTransform* tr = game_object_attached->parent->transform;
		
		if (tr != nullptr)
			global_transformation = tr->global_transformation * local_transformation;
		else
			global_transformation = local_transformation;
	}
	else 
	{
		global_transformation = local_transformation;
	}

	up = local_rotation.WorldY();

	forward = { 2 * (local_rotation.x * local_rotation.z + local_rotation.w * local_rotation.y),
				2 * (local_rotation.y * local_rotation.z - local_rotation.w * local_rotation.x),
				1 - 2 * (local_rotation.x * local_rotation.x + local_rotation.y * local_rotation.y) };

	right = { 1 - 2 * (local_rotation.y * local_rotation.y + local_rotation.z * local_rotation.z),
			 2 * (local_rotation.x * local_rotation.y + local_rotation.w * local_rotation.z),
			 2 * (local_rotation.x * local_rotation.z + local_rotation.w * local_rotation.y) };

	
	for (std::vector<GameObject*>::iterator item = game_object_attached->children.begin(); item != game_object_attached->children.end(); ++item) {
		if (*item != nullptr && (*item)->transform) 
		{
			(*item)->transform->RecalculateTransform();
		}
	}

	ComponentMesh* mesh = (ComponentMesh*)game_object_attached->GetComponent(ComponentType::MESH);
	if (mesh == nullptr)
		mesh = (ComponentMesh*)game_object_attached->GetComponent(ComponentType::DEFORMABLE_MESH);

	if (mesh != nullptr)
	{
		mesh->RecalculateAABB_OBB();
	}

	ComponentCurve* curve = game_object_attached->GetComponent<ComponentCurve>();
	if (curve != nullptr) {
		curve->UpdatePosition(GetGlobalPosition());
	}
}


bool ComponentTransform::DrawInspector()
{
	ImGui::Spacing();

	if (ImGui::Checkbox("Enabled", &game_object_attached->enabled))
		game_object_attached->SayChildrenParentIsEnabled(game_object_attached->enabled);
	ImGui::SameLine();

	static char name[30];
	memcpy(name, game_object_attached->GetName(), 30);

	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5F);

	if (ImGui::InputText("##ObjectName", name, 30, ImGuiInputTextFlags_AutoSelectAll)) {
		game_object_attached->SetName(name);
	}

	ImGui::SameLine();

	if (ImGui::Checkbox("Static", &game_object_attached->is_static)) {
		if (!game_object_attached->is_static && (game_object_attached->children.empty() || !game_object_attached->HasChildrenStatic())) {
			App->objects->octree.Remove(game_object_attached);
		}
		else if (game_object_attached->is_static && game_object_attached->parent != nullptr && game_object_attached->parent->is_static && game_object_attached->children.empty()) {
			App->objects->octree.Insert(game_object_attached, false);
		}
		else {
			popup_static = true;
		}
	}
	ImGui::Spacing();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6F);
	if (ImGui::BeginCombo("Tag", game_object_attached->tag))
	{
		std::vector<std::string>::iterator item = App->objects->tags.begin();
		for (; item != App->objects->tags.end(); ++item) {
			bool is_selected = App->StringCmp(game_object_attached->tag, (*item).data());
			if (ImGui::Selectable((*item).data(), is_selected)) {
				strcpy(game_object_attached->tag, (*item).data());
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	if (ImGui::Button("Custom Tags")) {
		popup_tags = true;
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (game_object_attached->IsPrefab() && !App->objects->prefab_scene && game_object_attached->FindPrefabRoot() == game_object_attached)
	{
		if (ImGui::CollapsingHeader("Prefab Options", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Spacing();

			if (ImGui::Button("Open Prefab"))
			{
				ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(game_object_attached->GetPrefabID());
				if (prefab != nullptr) {
					prefab->OpenPrefabScene();
					return false;
				}
			}

			ImGui::SameLine();
			ImGui::Text("|");
			ImGui::SameLine();

			if (ImGui::Button("Unpack Prefab"))
			{
				game_object_attached->FindPrefabRoot()->UnpackPrefab();
			}

			ImGui::SameLine();
			ImGui::Text("|");
			ImGui::SameLine();

			if (ImGui::Checkbox("Prefab Locked", &game_object_attached->prefab_locked))
			{
				game_object_attached->FindPrefabRoot()->LockPrefab(game_object_attached->prefab_locked);
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.1f);
			if (ImGui::Button("Select Prefab Root"))
			{
				App->objects->SetNewSelectedObject(game_object_attached->FindPrefabRoot(), false);
				App->camera->Focus();
				return false;
			}

			ImGui::SameLine();
			ImGui::Text("|");
			ImGui::SameLine();

			if (ImGui::Button("Select Prefab Asset"))
			{
				ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(game_object_attached->GetPrefabID());
				if (prefab != nullptr)
					App->ui->panel_project->SelectFile(prefab->GetAssetsPath(), App->resources->assets);
			}

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.06f);
			if (ImGui::Button("Set as the Original "))
			{
				GameObject* obj = game_object_attached->FindPrefabRoot();
				if (obj != nullptr) {
					std::vector<GameObject*>::iterator item = obj->parent->children.begin();
					for (; item != obj->parent->children.end(); ++item) {
						if (*item != nullptr && *item == obj) {
							ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(obj->GetPrefabID());
							if (prefab != nullptr) {
								std::vector<ComponentScript*> current_scripts;
								App->objects->GetRoot(true)->GetComponentsChildren(ComponentType::SCRIPT, (std::vector<Component*>*)&current_scripts, true);
								std::vector<std::pair<GameObject**, std::string>> objectsToAssign;
								for (auto scripts = current_scripts.begin(); scripts != current_scripts.end(); ++scripts) {
									if (!game_object_attached->Exists((*scripts)->game_object_attached)) {
										for (auto variables = (*scripts)->inspector_variables.begin(); variables != (*scripts)->inspector_variables.end(); ++variables) {
											if ((*variables).variable_type == InspectorScriptData::DataType::GAMEOBJECT && (*variables).obj != nullptr && game_object_attached->Exists(*(*variables).obj)) {
												GameObject* nameObj = *(*variables).obj;
												objectsToAssign.push_back({ (*variables).obj, nameObj->GetName() });
												*(*variables).obj = nullptr;
											}
										}
									}
								}

								(*item)->ToDelete();
								GameObject* newObj = prefab->ConvertToGameObjects(obj->parent, item - obj->parent->children.begin(), (*item)->GetComponent<ComponentTransform>()->local_position);
							
								if (!objectsToAssign.empty()) {
									std::stack<GameObject*> objects;
									objects.push(newObj);
									while (!objects.empty()) {
										GameObject* toCheck = objects.top();
										objects.pop();
										for (auto it = objectsToAssign.begin(); it != objectsToAssign.end(); ++it) {
											if (strcmp((*it).second.data(), toCheck->GetName()) == 0) {
												*(*it).first = toCheck;
											}
										}
										for (auto child = toCheck->children.begin(); child != toCheck->children.end(); ++child) {
											objects.push(*child);
										}
									}
								}
							}
							return false;
						}
					}
				}
			}

			ImGui::SameLine();
			ImGui::Text("|");
			ImGui::SameLine();

			/*ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.25);*/
			if (ImGui::Button("Save as the Original"))
			{
				ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(game_object_attached->GetPrefabID());
				if (prefab != nullptr) {
					prefab->Save(game_object_attached->FindPrefabRoot());
					App->objects->SetNewSelectedObject(game_object_attached, false);
					return false;
				}
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}
	}

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

		RightClickMenu("Transform");
		static bool set_cntrl_Z = true;
		ImGui::Spacing();
		ImGui::Text("Position  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(1);
		static float3 view_pos;
		view_pos = local_position;
		if (ImGui::DragFloat("X", &view_pos.x, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static)) {
			if (set_cntrl_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_cntrl_Z = false;
			local_position = view_pos;
			RecalculateTransform();
		}
		else if (!set_cntrl_Z && ImGui::IsMouseReleased(0)) {
			set_cntrl_Z = true;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(2);
		if (ImGui::DragFloat("Y", &view_pos.y, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static)) {
			if (set_cntrl_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_cntrl_Z = false;
			local_position = view_pos;
			RecalculateTransform();
		}
		else if (!set_cntrl_Z && ImGui::IsMouseReleased(0)) {
			set_cntrl_Z = true;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(3);
		if (ImGui::DragFloat("Z", &view_pos.z, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static)) {
			if (set_cntrl_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_cntrl_Z = false;
			local_position = view_pos;
			RecalculateTransform();
		}
		else if (!set_cntrl_Z && ImGui::IsMouseReleased(0)) {
			set_cntrl_Z = true;
		}
		ImGui::PopID();
		ImGui::Spacing();

		ImGui::Text("Rotation  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(4);
		static float3 view_rot;
		view_rot = euler_rotation;
		if (ImGui::DragFloat("X", &view_rot.x, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static)) {
			if (set_cntrl_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_cntrl_Z = false;
			euler_rotation = view_rot;
			float3 aux = euler_rotation;
			aux.x = DegToRad(euler_rotation.x);
			aux.y = DegToRad(euler_rotation.y);
			aux.z = DegToRad(euler_rotation.z);
			local_rotation = Quat::FromEulerXYZ(aux.x, aux.y, aux.z);
			RecalculateTransform();
		}
		else if (!set_cntrl_Z && ImGui::IsMouseReleased(0)) {
			set_cntrl_Z = true;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(5);
		if (ImGui::DragFloat("Y", &view_rot.y, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static)) {
			if (set_cntrl_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_cntrl_Z = false;
			euler_rotation = view_rot;
			float3 aux = euler_rotation;
			aux.x = DegToRad(euler_rotation.x);
			aux.y = DegToRad(euler_rotation.y);
			aux.z = DegToRad(euler_rotation.z);
			local_rotation = Quat::FromEulerXYZ(aux.x, aux.y, aux.z);
			RecalculateTransform();
		}
		else if (!set_cntrl_Z && ImGui::IsMouseReleased(0)) {
			set_cntrl_Z = true;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(6);
		if (ImGui::DragFloat("Z", &view_rot.z, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static)) {
			if (set_cntrl_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_cntrl_Z = false;
			euler_rotation = view_rot;
			float3 aux = euler_rotation;
			aux.x = DegToRad(euler_rotation.x);
			aux.y = DegToRad(euler_rotation.y);
			aux.z = DegToRad(euler_rotation.z);
			local_rotation = Quat::FromEulerXYZ(aux.x, aux.y, aux.z);
			RecalculateTransform();
		}
		else if (!set_cntrl_Z && ImGui::IsMouseReleased(0)) {
			set_cntrl_Z = true;
		}
		ImGui::PopID();
		ImGui::Spacing();

		ImGui::Text("Scale     ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(7);
		static float3 view_scale;
		view_scale = local_scale;
		if (ImGui::DragFloat("X", &view_scale.x, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static)) {
			if (set_cntrl_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_cntrl_Z = false;
			local_scale = view_scale;
			LookScale();
			RecalculateTransform();
		}
		else if (!set_cntrl_Z && ImGui::IsMouseReleased(0)) {
			set_cntrl_Z = true;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(8);
		if (ImGui::DragFloat("Y", &view_scale.y, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static)) {
			if (set_cntrl_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_cntrl_Z = false;
			local_scale = view_scale;
			LookScale();
			RecalculateTransform();
		}
		else if (!set_cntrl_Z && ImGui::IsMouseReleased(0)) {
			set_cntrl_Z = true;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(9);
		if (ImGui::DragFloat("Z", &view_scale.z, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static)) {
			if (set_cntrl_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_cntrl_Z = false;
			local_scale = view_scale;
			LookScale();
			RecalculateTransform();
		}
		else if (!set_cntrl_Z && ImGui::IsMouseReleased(0)) {
			set_cntrl_Z = true;
		}
		ImGui::PopID();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Transform");
	}

	if (popup_tags) {
		ImGui::OpenPopup("Customize Tags");
		ImGui::SetNextWindowSize({ 315,281 });
		static std::list<std::vector<std::string>::iterator> selected;
		if (ImGui::BeginPopupModal("Customize Tags", &popup_tags, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			static char new_tag[MAX_PATH] = "New Tag Name";
			if (ImGui::InputText("##New Tag", new_tag, MAX_PATH, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
				if (AddNewTagClicked(new_tag)) {
					strcpy(new_tag, "New Tag Name");
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Add New Tag")) {
				if (AddNewTagClicked(new_tag)) {
					strcpy(new_tag, "New Tag Name");
				}
			}
			ImGui::Spacing();
			ImGui::SetCursorPosX(115);
			ImGui::Text("Current Tags");
			ImGui::Spacing();
			ImGui::BeginChild("##CurrentTagss", { 0,170 }, true);

			std::vector<std::string>::iterator item = App->objects->tags.begin() + 1;
			for (; item != App->objects->tags.end(); ++item) {
				bool exists = (std::find(selected.begin(), selected.end(), item) != selected.end());
				ImGui::Selectable(std::string("##" + (*item)).data(), exists, ImGuiSelectableFlags_AllowItemOverlap);
				if (ImGui::IsItemClicked()) {
					if (exists) {
						selected.remove(item);
					}
					else {
						selected.push_back(item);
					}
				}
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5F - ImGui::CalcTextSize((*item).data()).x * 0.5F);
				ImGui::Text((*item).data());
			}

			ImGui::EndChild();
			ImGui::Spacing();
			ImGui::SetCursorPosX(193);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.6F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.7F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			if (ImGui::Button("Delete Selected")) {
				std::list<std::vector<std::string>::iterator>::iterator item = selected.begin();
				uint i = 0;
				for (; item != selected.end(); ++item) {
					App->objects->GetRoot(true)->ReTag((*(*item)).data(), "UnTagged");
					App->objects->tags.erase((*item) - i);
					++i;
				}
				selected.clear();

				remove(FILE_TAGS);
				JSON_Value* alien_value = json_value_init_object();
				JSON_Object* alien_object = json_value_get_object(alien_value);
				json_serialize_to_file_pretty(alien_value, FILE_TAGS);

				if (alien_value != nullptr && alien_object != nullptr) {

					JSONfilepack* alien = new JSONfilepack(FILE_TAGS, alien_object, alien_value);
					alien->StartSave();
					JSONArraypack* new_tags = alien->InitNewArray("Tags");
					auto item = App->objects->tags.begin() + 1;
					for (; item != App->objects->tags.end(); ++item) {
						new_tags->SetAnotherNode();
						new_tags->SetString("Tag", (*item).data());
					}
					alien->FinishSave();
					delete alien;
				}
			}
			ImGui::PopStyleColor(3);
			ImGui::EndPopup();
		}
		else {
			selected.clear();
		}
	}

	if (popup_static) {
		if (game_object_attached->is_static) {
			if (game_object_attached->parent != nullptr && !game_object_attached->parent->is_static) {
				// if your parent is dynamic, you cant be static
				ImGui::OpenPopup("Static Problems!");
				ImGui::SetNextWindowSize({ 290,140 });
				if (ImGui::BeginPopupModal("Static Problems!", &popup_static, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
				{
					ImGui::Spacing();

					ImGui::Text("Parent of selected object isn't static");
					ImGui::Spacing();
					ImGui::Text("GameObject: %s", game_object_attached->GetName());
					ImGui::Spacing();
					ImGui::Text("Parent: %s", game_object_attached->parent->GetName());

					ImGui::Spacing();

					ImGui::Text("Make parent static to set this static");
					ImGui::SetCursorPosX(((ImGui::GetWindowWidth()) * 0.5f) - 50);

					if (ImGui::Button("Accept", { 100,20 })) {
						popup_static = false;
						game_object_attached->is_static = false;
					}
					ImGui::EndPopup();
				}
				else {
					popup_static = false;
					game_object_attached->is_static = false;
				}
			}
			else {
				ImGui::OpenPopup("Static Question");
				ImGui::SetNextWindowSize({ 270,120 });
				if (ImGui::BeginPopupModal("Static Question", &popup_static, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
				{
					ImGui::Spacing();
					ImGui::SetCursorPosX(20);
					ImGui::Text("Do you want to make all children\nstatic as well?");
					ImGui::Spacing();

					ImGui::SetCursorPosX(12);
					if (ImGui::Button("Change children")) {
						game_object_attached->ChangeStatic(game_object_attached->is_static);
						App->objects->octree.Insert(game_object_attached, true);
						popup_static = false;
					}
					ImGui::SameLine();
					if (ImGui::Button("Only this object")) {
						App->objects->octree.Insert(game_object_attached, false);
						popup_static = false;
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX(((ImGui::GetWindowWidth()) * 0.5f) - 35);
					if (ImGui::Button("Cancel")) {
						game_object_attached->is_static = false;
						popup_static = false;
					}

					ImGui::EndPopup();
				}
				else {
					game_object_attached->is_static = false;
					popup_static = false;
				}
			}
		}
		else {
			// if you make a object dynamic, children will transform to dynamic
			ImGui::OpenPopup("Static Question");
			ImGui::SetNextWindowSize({ 290,150 });
			if (ImGui::BeginPopupModal("Static Question", &popup_static, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
			{
				ImGui::Spacing();
				ImGui::SetCursorPosX(20);
				ImGui::Text("To set this object dynamic, you must\nset children dynamic too");
				ImGui::SetCursorPosX(20);
				ImGui::Text("Do you want to make all children\ndynamic as well?");
				ImGui::Spacing();
				ImGui::SetCursorPosX(20);
				ImGui::Text("GameObject: %s", game_object_attached->GetName());

				ImGui::Spacing();

				ImGui::SetCursorPosX(37);

				if (ImGui::Button("Yes, change children")) {
					popup_static = false;
					game_object_attached->ChangeStatic(game_object_attached->is_static);
					App->objects->octree.Remove(game_object_attached);
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					popup_static = false;
					game_object_attached->is_static = true;
				}
				ImGui::EndPopup();
			}
			else {
				popup_static = false;
				game_object_attached->is_static = true;
			}
		}
	}
	return true;
}

bool ComponentTransform::AddNewTagClicked(const char* new_tag)
{
	std::vector<std::string>::iterator item = App->objects->tags.begin();
	for (; item != App->objects->tags.end(); ++item) {
		if (App->StringCmp((*item).data(), new_tag)) {
			return false;
		}
	}

	App->objects->tags.push_back(new_tag);
	remove(FILE_TAGS);
	JSON_Value* alien_value = json_value_init_object();
	JSON_Object* alien_object = json_value_get_object(alien_value);
	json_serialize_to_file_pretty(alien_value, FILE_TAGS);

	if (alien_value != nullptr && alien_object != nullptr) {

		JSONfilepack* alien = new JSONfilepack(FILE_TAGS, alien_object, alien_value);
		alien->StartSave();
		JSONArraypack* new_tags = alien->InitNewArray("Tags");
		auto item = App->objects->tags.begin() + 1;
		for (; item != App->objects->tags.end(); ++item) {
			new_tags->SetAnotherNode();
			new_tags->SetString("Tag", (*item).data());
		}
		alien->FinishSave();
		delete alien;
	}
	return true;
}

void ComponentTransform::SetScaleNegative(const bool& negative)
{
	is_scale_negative = negative;
}

bool ComponentTransform::IsScaleNegative()
{
	return is_scale_negative;
}

void ComponentTransform::Reset()
{
	local_scale = { 1,1,1 };
	local_position = { 0,0,0 };
	local_rotation = { 0,0,0,0 };

	euler_rotation = local_rotation.ToEulerXYZ();
	euler_rotation.x = RadToDeg(euler_rotation.x);
	euler_rotation.y = RadToDeg(euler_rotation.y);
	euler_rotation.z = RadToDeg(euler_rotation.z);

	is_scale_negative = false;

	RecalculateTransform();
}

void ComponentTransform::SetComponent(Component* component)
{
	if (component->GetType() == type) {

		ComponentTransform* transform = (ComponentTransform*)component;

		local_position = transform->local_position;
		local_scale = transform->local_scale;
		local_rotation = transform->local_rotation;
		euler_rotation = transform->euler_rotation;

		LookScale();
		RecalculateTransform();
	}
}

void ComponentTransform::Clone(Component* clone)
{
	clone->enabled = enabled;
	clone->not_destroy = not_destroy;
	ComponentTransform* transform = (ComponentTransform*)clone;
	transform->euler_rotation = euler_rotation;
	transform->forward = forward;
	transform->global_transformation = global_transformation;
	transform->is_scale_negative = is_scale_negative;
	transform->local_position = local_position;
	transform->local_rotation = local_rotation;
	transform->local_scale = local_scale;
	transform->local_transformation = local_transformation;
	transform->right = right;
	transform->up = up;
}

void ComponentTransform::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetFloat3("Position", local_position);
	to_save->SetQuat("Rotation", local_rotation);
	to_save->SetFloat3("Scale", local_scale);
	to_save->SetBoolean("ScaleNegative", is_scale_negative);
	to_save->SetString("ID", std::to_string(ID).data());
}

void ComponentTransform::LoadComponent(JSONArraypack* to_load)
{
	local_position = to_load->GetFloat3("Position");
	local_rotation = to_load->GetQuat("Rotation");
	local_scale = to_load->GetFloat3("Scale");
	is_scale_negative = to_load->GetBoolean("ScaleNegative");
	ID = std::stoull(to_load->GetString("ID"));
	euler_rotation = local_rotation.ToEulerXYZ();
	euler_rotation.x = RadToDeg(euler_rotation.x);
	euler_rotation.y = RadToDeg(euler_rotation.y);
	euler_rotation.z = RadToDeg(euler_rotation.z);

	local_transformation = float4x4::FromTRS(local_position, local_rotation, local_scale);

	if (game_object_attached->parent != nullptr) {
		ComponentTransform* tr = (ComponentTransform*)game_object_attached->parent->GetComponent(ComponentType::TRANSFORM);
		if (tr != nullptr) global_transformation = tr->global_transformation * local_transformation;
		else global_transformation = local_transformation;
	}
	else
		global_transformation = local_transformation;
}

void ComponentTransform::SetGlobalTransformation(const float4x4& global_transformation)
{
	float3 position, scale;
	Quat rotation;
	global_transformation.Decompose(position, rotation, scale);

	if (App->ui->panel_scene->guizmo_operation == ImGuizmo::OPERATION::SCALE) { // why... i would like to know it...
		this->local_scale = scale;
		LookScale();
	}
	else {
		this->local_position = position;
		this->local_rotation = rotation;
		euler_rotation = local_rotation.ToEulerXYZ();
		euler_rotation.x = RadToDeg(euler_rotation.x);
		euler_rotation.y = RadToDeg(euler_rotation.y);
		euler_rotation.z = RadToDeg(euler_rotation.z);
	}

	RecalculateTransform();
}

void ComponentTransform::AddPosition(const float3 pos)
{
	local_position += pos;
	RecalculateTransform();
}

void ComponentTransform::AddScale(const float3 scale)
{
	local_scale += scale;
	LookScale();
	RecalculateTransform();
}

void ComponentTransform::AddRotation(const float3 rot)
{
	euler_rotation += rot;
	float3 aux = euler_rotation;
	aux.x = DegToRad(euler_rotation.x);
	aux.y = DegToRad(euler_rotation.y);
	aux.z = DegToRad(euler_rotation.z);
	local_rotation = Quat::FromEulerXYZ(aux.x, aux.y, aux.z);

	RecalculateTransform();
}

void ComponentTransform::Reparent(const float4x4& transform)
{
	float3 position, scale;
	Quat rotation;
	transform.Decompose(position, rotation, scale);

	this->local_scale = scale;
	LookScale();
	this->local_position = position;
	this->local_rotation = rotation;
	euler_rotation = local_rotation.ToEulerXYZ();
	euler_rotation.x = RadToDeg(euler_rotation.x);
	euler_rotation.y = RadToDeg(euler_rotation.y);
	euler_rotation.z = RadToDeg(euler_rotation.z);

	RecalculateTransform();
}