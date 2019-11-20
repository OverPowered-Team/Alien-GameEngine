#include "PanelHierarchy.h"
#include "ModuleObjects.h"
#include "GameObject.h"
#include "PanelCreateObject.h"
#include "ResourcePrefab.h"
#include "imgui/imgui_internal.h"
#include "PanelProject.h"
#include "ResourceTexture.h"


PanelHierarchy::PanelHierarchy(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Hierarchy", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
}


PanelHierarchy::~PanelHierarchy()
{
}

void PanelHierarchy::PanelLogic()
{
	ImGui::Begin(panel_name.data(), &enabled, ImGuiWindowFlags_NoCollapse);

	if (ImGui::IsWindowHovered()) {
		App->camera->is_scene_hovered = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_DELETE) && App->objects->GetSelectedObject() != nullptr)
	{
		if (App->objects->GetSelectedObject()->IsPrefab() && App->objects->GetSelectedObject()->FindPrefabRoot() != App->objects->GetSelectedObject())
			popup_prefab_reparent = true;
		else App->objects->GetSelectedObject()->ToDelete();
	}
	
	ImGui::Spacing();
	if (App->objects->prefab_scene) {
		std::string name = std::string("Prefab Scene: " + std::string(App->objects->GetRoot(false)->GetName())).data();
		ImVec2 size = ImGui::CalcTextSize(name.data());
		ImGui::SetCursorPos({ ImGui::GetWindowWidth() * 0.5f - size.x * 0.5f ,ImGui::GetCursorPosY() });
		ImGui::Text(name.data());
	}
	else
		ImGui::Text(std::string("Current Scene: " + App->objects->current_scene.name_without_extension).data());
	ImGui::Spacing();
	
	if (App->objects->prefab_scene) {
		ImGui::SetCursorPos({ ImGui::GetWindowWidth() * 0.5f - 45,ImGui::GetCursorPosY() });
		if (ImGui::Button("Return Scene")) {
			App->objects->prefab_scene = false;
			App->objects->LoadScene("Library/save_prefab_scene.alienScene", false);
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	object_hovered = nullptr;
	if (!App->objects->GetRoot(true)->children.empty()) {
		std::vector<GameObject*>::iterator item = App->objects->GetRoot(true)->children.begin();
		for (; item != App->objects->GetRoot(true)->children.end(); ++item)
		{
			if (*item != nullptr)
			{
				PrintNode(*item);
			}
		}
	}
	RightClickMenu();

	// drop a node in the window, parent is base_game_object
	ImVec2 min_space = ImGui::GetWindowContentRegionMin();
	ImVec2 max_space = ImGui::GetWindowContentRegionMax();
	
	min_space.x += ImGui::GetWindowPos().x;
	min_space.y += ImGui::GetWindowPos().y;
	max_space.x += ImGui::GetWindowPos().x;
	max_space.y += ImGui::GetWindowPos().y;

	if (ImGui::BeginDragDropTargetCustom({ min_space.x,min_space.y, max_space.x,max_space.y }, ImGui::GetID(panel_name.data()))) {
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover | ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
		if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
			GameObject* obj = *(GameObject**)payload->Data;
			if (obj != nullptr) {
				if (obj->IsPrefab() && obj->FindPrefabRoot() != obj)
					popup_prefab_reparent = true;
				else if (!obj->is_static)
					App->objects->ReparentGameObject(obj, App->objects->GetRoot(false));
				else
					LOG("Objects static can not be reparented");
			}
			ImGui::ClearDragDrop();
		}
		ImGui::EndDragDropTarget();
	}
	if (popup_prefab_reparent) {
		ImGui::OpenPopup("Can not change prefab instance");
		ImGui::SetNextWindowSize({ 240,130 });
		if (ImGui::BeginPopupModal("Can not change prefab instance", &popup_prefab_reparent, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::SetCursorPosX(30);
			ImGui::Text("You can unpack the Prefab");
			ImGui::Text("instance to remove its conexion");
			ImGui::Spacing();
			ImGui::SetCursorPosX(35);
			ImGui::Text("Children of a Prefab can\nnot be deleted or moved.");
			ImGui::Spacing();
			ImGui::SetCursorPosX(95);
			if (ImGui::Button("Ok", {50,0})) {
				popup_prefab_reparent = false;
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
	
}

void PanelHierarchy::PrintNode(GameObject* node)
{
	// active checkbox
	ImGui::PushID(node);
	if (ImGui::Checkbox("##Active", &node->enabled)) {
		node->SayChildrenParentIsEnabled(node->enabled);
	}
	ImGui::PopID();

	ImGui::SameLine();

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
	if (node->IsPrefab() && node->FindPrefabRoot() == node)  ImGui::Image((ImTextureID)App->resources->icons.prefab->id, ImVec2(15, 15));
	else ImGui::Image((ImTextureID)App->resources->icons.box->id, ImVec2(15, 15));

	ImGui::SameLine();

	ImGui::PushID(node);
	
	if (node->open_node) {
		node->open_node = false;
		ImGui::SetNextItemOpen(true);
	}

	bool is_tree_open = ImGui::TreeNodeEx(node->GetName(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | 
		ImGuiTreeNodeFlags_OpenOnDoubleClick | (node->IsSelected() ? ImGuiTreeNodeFlags_Selected : 0) | 
		(node->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0), (!node->IsEnabled() || !node->IsParentEnabled()));

	if (ImGui::IsItemClicked()) {
		App->objects->SetNewSelectedObject(node);
	}
	if (ImGui::IsItemHovered()) {
		object_hovered = node;
	}

	if (ImGui::BeginDragDropTarget()) {
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
		if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
			GameObject* obj = *(GameObject**)payload->Data;
			if (obj != nullptr) {
				if (obj->IsPrefab() && obj->FindPrefabRoot() != obj)
					popup_prefab_reparent = true;
				else if (!obj->is_static)
					App->objects->ReparentGameObject(obj, node);
				else
					LOG("Objects static can not be reparented");
			}
			ImGui::ClearDragDrop();
		}
		ImGui::EndDragDropTarget();
	}
	
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover)) {
		ImGui::SetDragDropPayload(DROP_ID_HIERARCHY_NODES, &node, sizeof(GameObject), ImGuiCond_Once);
		ImGui::Text(node->GetName());
		ImGui::EndDragDropSource();
	}

	if (is_tree_open) {
		std::vector<GameObject*>::iterator item = node->children.begin();
		for (; item != node->children.end(); ++item)
		{
			if (*item != nullptr)
			{
				PrintNode(*item);
			}
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}
void PanelHierarchy::RightClickMenu()
{
	if (ImGui::BeginPopupContextWindow()) {

		if (!in_menu) {
			in_menu = true;
			object_menu = object_hovered;
			if (object_menu != nullptr) {
				App->objects->SetNewSelectedObject(object_menu);
			}
		}
		if (object_menu != nullptr) {

			bool is_on_top = false;
			bool is_on_bottom = false;

			if (object_menu->parent->children.front() == object_menu)
				is_on_top = true;
			if (object_menu->parent->children.back() == object_menu)
				is_on_bottom = true;

			if (ImGui::MenuItem("Move One Up", nullptr, nullptr, !is_on_top))
			{
				App->objects->MoveObjectUp(object_menu, false);
			}

			if (ImGui::MenuItem("Move On Top", nullptr, nullptr, !is_on_top))
			{
				App->objects->MoveObjectUp(object_menu, true);
			}

			if (ImGui::MenuItem("Move One Down", nullptr, nullptr, !is_on_bottom))
			{
				App->objects->MoveObjectDown(object_menu, false);
			}

			if (ImGui::MenuItem("Move On Bottom", nullptr, nullptr, !is_on_bottom))
			{
				App->objects->MoveObjectDown(object_menu, true);
			}

			if (ImGui::MenuItem("Make it Pefab")) {
				ResourcePrefab* prefab = new ResourcePrefab();
				prefab->CreateMetaData(object_menu);
				App->ui->panel_project->RefreshAllNodes();
			}

			if (object_menu->IsPrefab()) {
				ImGui::Separator();

				if (ImGui::MenuItem("Select Prefab Root")) {
					App->objects->SetNewSelectedObject(object_menu->FindPrefabRoot());
					App->camera->Focus();
				}

				if (ImGui::MenuItem("Open Prefab", nullptr, nullptr, !App->objects->prefab_scene)) {
					ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(object_menu->GetPrefabID());
					if (prefab != nullptr)
						prefab->OpenPrefabScene();
				}

				if (ImGui::MenuItem("Select Prefab Asset")) {
					// TODO:
				}

				if (ImGui::MenuItem("Set Prefab as the Original", nullptr, nullptr, !App->objects->prefab_scene)) {
					GameObject* obj = object_menu->FindPrefabRoot();
					if (obj != nullptr) {
						std::vector<GameObject*>::iterator item = obj->parent->children.begin();
						for (; item != obj->parent->children.end(); ++item) {
							if (*item != nullptr && *item == obj) {
								ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(obj->GetPrefabID());
								if (prefab != nullptr) {
									(*item)->ToDelete();
									prefab->ConvertToGameObjects(item - obj->parent->children.begin());
								}
								break;
							}
						}
					}
				}

				if (ImGui::MenuItem("UnPack Prefab", nullptr, nullptr, !App->objects->prefab_scene)) {
					GameObject* obj = object_menu->FindPrefabRoot();
					obj->UnpackPrefab();
				}
			}


			ImGui::Separator();

			if (ImGui::MenuItem("Create Empty Child"))
			{
				App->objects->CreateEmptyGameObject(object_menu);
			}
			if (ImGui::MenuItem("Remove Object"))
			{
				object_menu->ToDelete();
			}
			ImGui::Separator();
		}
		if (ImGui::MenuItem("Crete Empty GameObject"))
		{
			App->objects->CreateEmptyGameObject(nullptr);
		}
		if (ImGui::MenuItem("Cube"))
		{
			App->objects->CreateBasePrimitive(PrimitiveType::CUBE);
		}
		if (ImGui::MenuItem("Sphere"))
		{
			App->objects->CreateBasePrimitive(PrimitiveType::SPHERE_ALIEN);
		}
		if (ImGui::MenuItem("Rock"))
		{
			App->objects->CreateBasePrimitive(PrimitiveType::ROCK);
		}
		if (ImGui::MenuItem("Torus"))
		{
			App->objects->CreateBasePrimitive(PrimitiveType::TORUS);
		}
		if (ImGui::BeginMenu("Other"))
		{
			if (ImGui::MenuItem("Dodecahedron"))
			{
				App->objects->CreateBasePrimitive(PrimitiveType::DODECAHEDRON);
			}
			if (ImGui::MenuItem("Octahedron"))
			{
				App->objects->CreateBasePrimitive(PrimitiveType::OCTAHEDRON);
			}
			if (ImGui::MenuItem("Icosahedron"))
			{
				App->objects->CreateBasePrimitive(PrimitiveType::ICOSAHEDRON);
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Create..."))
		{
			App->ui->panel_create_object->ChangeEnable();
		}
		ImGui::EndPopup();
	}
	else if (in_menu) {
		in_menu = false;
		object_menu = nullptr;
	}

}