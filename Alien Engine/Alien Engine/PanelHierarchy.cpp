#include "PanelHierarchy.h"
#include "ModuleObjects.h"
#include "GameObject.h"
#include "PanelCreateObject.h"
#include "ResourceScene.h"
#include "ResourcePrefab.h"
#include "imgui/imgui_internal.h"
#include "ComponentScript.h"
#include "PanelSceneSelector.h"
#include "PanelProject.h"
#include "ResourceTexture.h"
#include "ComponentTransform.h"
#include "ModuleResources.h"
#include "ShortCutManager.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleUI.h"
#include "mmgr/mmgr.h"
#include "Optick/include/optick.h"

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
	OPTICK_EVENT();
	ImGui::Begin(panel_name.data(), &enabled, ImGuiWindowFlags_NoCollapse);

	if (ImGui::IsWindowHovered()) {
		App->camera->is_scene_hovered = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_DELETE) && !App->objects->GetSelectedObjects().empty())
	{
		std::list<GameObject*> selected = App->objects->GetSelectedObjects();
		if (!App->objects->prefab_scene) {
			auto item = selected.begin();
			for (; item != selected.end(); ++item) {
				if (*item != nullptr && (*item)->IsPrefab() && (*item)->FindPrefabRoot() != *item) {
					popup_prefab_restructurate = true;
				}
			}
		}
		else {
			auto item = selected.begin();
			for (; item != selected.end(); ++item) {
				if (*item != nullptr && (*item)->IsPrefab() && (*item)->FindPrefabRoot() == *item) {
					popup_delete_root_prefab_scene = true;
				}
			}
		}
		if (!popup_prefab_restructurate && !popup_delete_root_prefab_scene) {
			auto item = selected.begin();
			for (; item != selected.end(); ++item) {
				if (*item != nullptr) {
					(*item)->ToDelete();
				}
			}
			App->objects->DeselectObjects();
		}
	}
	
	ImGui::Spacing();
	if (App->objects->prefab_scene) {
		std::string name = std::string("Prefab Scene: " + std::string(App->objects->GetRoot(false)->GetName())).data();
		ImVec2 size = ImGui::CalcTextSize(name.data());
		ImGui::SetCursorPos({ ImGui::GetWindowWidth() * 0.5f - size.x * 0.5f ,ImGui::GetCursorPosY() });
		ImGui::Text(name.data());
		ImGui::Spacing();
		ImGui::SetCursorPos({ ImGui::GetWindowWidth() * 0.5f - 45,ImGui::GetCursorPosY() });
		if (ImGui::Button("Return Scene")) {
			popup_leave_prefab_view = true;
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		object_hovered = nullptr;
		if (!App->objects->GetGlobalRoot()->children.empty()) {
			std::vector<GameObject*>::iterator item = App->objects->GetGlobalRoot()->children.begin();
			for (; item != App->objects->GetGlobalRoot()->children.end(); ++item)
			{
				PrintNode(*item);
			}
		}

	}
	else {
		object_hovered = nullptr;
		if (!App->objects->GetGlobalRoot()->children.empty()) {
			std::vector<GameObject*>::iterator item = App->objects->GetGlobalRoot()->children.begin();
			for (; item != App->objects->GetGlobalRoot()->children.end(); ++item)
			{
				if (App->objects->GetGlobalRoot()->children.size() > 1 && (*item)->ID == App->objects->scene_active) {
					ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_HeaderActive));
				}

				ImGui::PushID(*item);
				if (ImGui::CollapsingHeader((*item)->name, ImGuiTreeNodeFlags_DefaultOpen)) {

					if (App->objects->GetGlobalRoot()->children.size() > 1 && (*item)->ID == App->objects->scene_active) {
						ImGui::PopStyleColor();
					}

					RightClickSceneNode(*item);

					if (ImGui::BeginDragDropTarget()) {
						const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
						if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
							GameObject* obj = *(GameObject**)payload->Data;
							std::vector<GameObject*> objects;
							if (!obj->IsSelected()) {
								objects.push_back(obj);
							}
							else {
								objects.assign(App->objects->GetSelectedObjects().begin(), App->objects->GetSelectedObjects().end());
							}
							for (auto it = objects.begin(); it != objects.end(); ++it) {
								if ((*it)->parent != *item) {
									if ((*it)->IsPrefab() && (*it)->FindPrefabRoot() != (*it) && !App->objects->prefab_scene) {
										popup_move_child_outof_root_prefab_scene = true;
									}
									else if (!(*it)->is_static) {
										App->objects->ReparentGameObject((*it), *item);
									}
									else {
										LOG_ENGINE("Objects static can not be reparented");
									}
								}
							}
							ImGui::ClearDragDrop();
						}
						ImGui::EndDragDropTarget();
					}

					for (auto it = (*item)->children.begin(); it != (*item)->children.end(); ++it) {
						PrintNode(*it);
					}
				}
				else {
					if (App->objects->GetGlobalRoot()->children.size() > 1 && (*item)->ID == App->objects->scene_active) {
						ImGui::PopStyleColor();
					}
					RightClickSceneNode(*item);
				}
				ImGui::PopID();

			}
		}
	}
	
	RightClickMenu();
	right_click_scene = false;

	// drop a node in the window, parent is base_game_object
	ImVec2 min_space = ImGui::GetWindowContentRegionMin();
	ImVec2 max_space = ImGui::GetWindowContentRegionMax();
	
	min_space.x += ImGui::GetWindowPos().x;
	min_space.y += ImGui::GetWindowPos().y;
	max_space.x += ImGui::GetWindowPos().x;
	max_space.y += ImGui::GetWindowPos().y;

	if (popup_prefab_restructurate) {
		ImGui::OpenPopup("Can not change prefab instance");
		ImGui::SetNextWindowSize({ 240,130 });
		if (ImGui::BeginPopupModal("Can not change prefab instance", &popup_prefab_restructurate, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
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
				popup_prefab_restructurate = false;
			}
			ImGui::EndPopup();
		}
	}

	if (popup_move_child_outof_root_prefab_scene) {
		ImGui::OpenPopup("Respect Prefab Root");
		ImGui::SetNextWindowSize({ 240,100 });
		if (ImGui::BeginPopupModal("Respect Prefab Root", &popup_move_child_outof_root_prefab_scene, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::SetCursorPosX(15);
			ImGui::Text("GameObjects can not be outside");
			ImGui::SetCursorPosX(70);
			ImGui::Text("the Prefab root");
			ImGui::Spacing();
			ImGui::SetCursorPosX(95);
			if (ImGui::Button("Ok", { 50,0 })) {
				popup_move_child_outof_root_prefab_scene = false;
			}
			ImGui::EndPopup();
		}
	}

	if (popup_delete_root_prefab_scene) {
		ImGui::OpenPopup("Respect Prefab Root");
		ImGui::SetNextWindowSize({ 240,80 });
		if (ImGui::BeginPopupModal("Respect Prefab Root", &popup_delete_root_prefab_scene, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::SetCursorPosX(15);
			ImGui::Text("Prefab Root can not be deleted");
			ImGui::Spacing();
			ImGui::SetCursorPosX(95);
			if (ImGui::Button("Ok", { 50,0 })) {
				popup_delete_root_prefab_scene = false;
			}
			ImGui::EndPopup();
		}
	}

	if (popup_no_open_prefab) {
		ImGui::OpenPopup("Prefab View no available");
		ImGui::SetNextWindowSize({ 245,80 });
		if (ImGui::BeginPopupModal("Prefab View no available", &popup_no_open_prefab, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Exit game to see the prefab view");
			ImGui::Spacing();
			ImGui::SetCursorPosX(95);
			if (ImGui::Button("Ok", { 50,0 })) {
				popup_no_open_prefab = false;
			}
			ImGui::EndPopup();
		}
	}

	if (popup_leave_prefab_view) {
		ImGui::OpenPopup("Edit Prefab Options");
		ImGui::SetNextWindowSize({ 170,100 });
		if (ImGui::BeginPopupModal("Edit Prefab Options", &popup_leave_prefab_view, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::SetCursorPosX(25);
			ImGui::Text("Save new changes?");
			ImGui::Spacing();
			ImGui::SetCursorPosX(12);
			
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });

			if (ImGui::Button("Don't save")) {
				popup_leave_prefab_view = false;
				App->objects->prefab_scene = false;
				App->objects->SwapReturnZ(true, true);
				App->objects->LoadScene("Library/save_prefab_scene.alienScene", false);
				App->objects->enable_instancies = true;
				remove("Library/save_prefab_scene.alienScene");
			}
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::SameLine();
			if (ImGui::Button("Save", { 60,0 })) {
				popup_leave_prefab_view = false;
				App->objects->prefab_opened->Save(App->objects->GetRoot(false));
			}
			//ImGui::Spacing();
			ImGui::SetCursorPosX(12);
			if (ImGui::Button("Cancel", { 145,0 })) {
				popup_leave_prefab_view = false;
			}

			ImGui::EndPopup();
		}
	}

	if (ImGui::IsWindowFocused()) {
		is_focused = true;
	}
	else {
		is_focused = false;
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
	if (node->IsPrefab() && node->FindPrefabRoot() == node)
	{
		if (node->prefab_locked)
		{
			ImGui::Image((ImTextureID)App->resources->icons.prefab_lock->id, ImVec2(15, 15));
		}
		else
		{
			ImGui::Image((ImTextureID)App->resources->icons.prefab->id, ImVec2(15, 15));
		}
	}
		
	else ImGui::Image((ImTextureID)App->resources->icons.box->id, ImVec2(15, 15));

	ImGui::SameLine();

	ImGui::PushID(node);
	
	if (node->open_node) {
		node->open_node = false;
		ImGui::SetNextItemOpen(true);
	}
	if (node->IsPrefab() && node->FindPrefabRoot() != node)
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, { (float)222 / 255,(float)100 / 255,1,1 });
	bool is_tree_open = ImGui::TreeNodeEx(node->GetName(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | 
		ImGuiTreeNodeFlags_OpenOnDoubleClick | (node->IsSelected() ? ImGuiTreeNodeFlags_Selected : 0) | 
		(node->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0), (!node->IsEnabled() || !node->IsUpWardsEnabled()));
	if (node->IsPrefab() && node->FindPrefabRoot() != node)
		ImGui::PopStyleColor();

	if (ImGui::IsItemHovered()) {
		object_hovered = node;
	}

	if (ImGui::BeginDragDropTarget()) {
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
		if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
			GameObject* obj = *(GameObject**)payload->Data;
			std::vector<GameObject*> objects;
			if (!obj->IsSelected()) {
				objects.push_back(obj);
			}
			else {
				objects.assign(App->objects->GetSelectedObjects().begin(), App->objects->GetSelectedObjects().end());
			}
			for (auto item = objects.begin(); item != objects.end(); ++item) {
				if ((*item) != nullptr) {
					if (!App->objects->prefab_scene && (*item)->IsPrefab() && (*item)->FindPrefabRoot() != (*item)) {
						popup_prefab_restructurate = true;
					}
					else if (!(*item)->is_static) {
						App->objects->ReparentGameObject((*item), node);
						node->open_node = true;
					}
					else {
						LOG_ENGINE("Objects static can not be reparented");
					}
				}
			}
			ImGui::ClearDragDrop();
		}
		ImGui::EndDragDropTarget();
	}
	
	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) {
		App->objects->SetNewSelectedObject(node, App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT);
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
	if (!right_click_scene && ImGui::BeginPopupContextWindow()) {

		if (!in_menu) {
			in_menu = true;
			object_menu = object_hovered;
			if (object_menu != nullptr) {
				App->objects->SetNewSelectedObject(object_menu, false);
			}
		}
		if (ImGui::MenuItem("Create New Script")) {
			App->ui->creating_script = true;
		}
		ImGui::Separator();
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

			if (ImGui::MenuItem("Make it Pefab", nullptr, nullptr, !App->objects->prefab_scene)) {
				ResourcePrefab* prefab = new ResourcePrefab();
				prefab->CreateMetaData(object_menu);
				App->ui->panel_project->RefreshAllNodes();
			}

			if (object_menu->IsPrefab()) {
				ImGui::Separator();

				if (ImGui::MenuItem("Select Prefab Root")) {
					App->objects->SetNewSelectedObject(object_menu->FindPrefabRoot(), false);
					App->camera->Focus();
				}

				if (ImGui::MenuItem("Open Prefab", nullptr, nullptr, !App->objects->prefab_scene)) {
					ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(object_menu->GetPrefabID());
					if (prefab != nullptr)
						prefab->OpenPrefabScene();
				}

				if (ImGui::MenuItem("Select Prefab Asset")) {
					ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(object_menu->GetPrefabID());
					if (prefab != nullptr)
						App->ui->panel_project->SelectFile(prefab->GetAssetsPath(), App->resources->assets);
				}

				if (object_menu->prefab_locked) {
					if (ImGui::MenuItem("Unlock Prefab", nullptr, nullptr, !App->objects->prefab_scene)) {
						object_menu->FindPrefabRoot()->LockPrefab(false);
					}
				}
				else {
					if (ImGui::MenuItem("Lock Prefab", nullptr, nullptr, !App->objects->prefab_scene)) {
						object_menu->FindPrefabRoot()->LockPrefab(true);
					}
				}

				if (ImGui::MenuItem("Set Prefab as the Original", nullptr, nullptr, !App->objects->prefab_scene)) {
					GameObject* obj = object_menu->FindPrefabRoot();
					if (obj != nullptr) {
						std::vector<GameObject*>::iterator item = obj->parent->children.begin();
						for (; item != obj->parent->children.end(); ++item) {
							if (*item != nullptr && *item == obj) {
								ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(obj->GetPrefabID());
								if (prefab != nullptr) {
									std::vector<ComponentScript*> current_scripts;
									App->objects->GetRoot(true)->GetComponentsChildren(ComponentType::SCRIPT, (std::vector<Component*>*) & current_scripts, true);
									std::vector<std::pair<GameObject**, std::string>> objectsToAssign;
									for (auto scripts = current_scripts.begin(); scripts != current_scripts.end(); ++scripts) {
										if (!(*item)->Exists((*scripts)->game_object_attached)) {
											for (auto variables = (*scripts)->inspector_variables.begin(); variables != (*scripts)->inspector_variables.end(); ++variables) {
												if ((*variables).variable_type == InspectorScriptData::DataType::GAMEOBJECT && (*variables).obj != nullptr && (*item)->Exists(*(*variables).obj)) {
													GameObject* nameObj = *(*variables).obj;
													objectsToAssign.push_back({ (*variables).obj, nameObj->GetName() });
													*(*variables).obj = nullptr;
												}
											}
										}
									}

									(*item)->ToDelete();
									GameObject* newObj = prefab->ConvertToGameObjects(obj->parent, item - obj->parent->children.begin(), obj->GetComponent<ComponentTransform>()->GetGlobalPosition());
								
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
								break;
							}
						}
					}
				}

				if (ImGui::MenuItem("Save Prefab as the Original", nullptr, nullptr, !App->objects->prefab_scene)) {
					ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(object_menu->GetPrefabID());
					if (prefab != nullptr) {
						prefab->Save(object_menu->FindPrefabRoot());
						App->objects->SetNewSelectedObject(object_menu, false);
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
				if (!App->objects->prefab_scene && object_menu->IsPrefab() && object_menu->FindPrefabRoot() != object_menu) {
					popup_prefab_restructurate = true;
				}
				else if (App->objects->prefab_scene && object_menu->IsPrefab() && object_menu->FindPrefabRoot() == object_menu) {
					popup_delete_root_prefab_scene = true;
				}
				else {
					object_menu->ToDelete();
				}
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
		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Point light"))
			{
				App->objects->CreateLight(LightTypeObj::POINT);
			}
			if (ImGui::MenuItem("Spot light"))
			{
				App->objects->CreateLight(LightTypeObj::SPOT);
			}
			if (ImGui::MenuItem("Directional light"))
			{
				App->objects->CreateLight(LightTypeObj::DIRECTIONAL);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Effects"))
		{
			if (ImGui::MenuItem("Particle System"))
			{
				App->objects->CreateEffect(ComponentType::PARTICLES);
			}
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("UI"))
		{
			if (ImGui::MenuItem("Image"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_IMAGE);
			}
			if (ImGui::MenuItem("Button"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_BUTTON);
			}
			if (ImGui::MenuItem("Checkbox"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_CHECKBOX);
			}
			if (ImGui::MenuItem("Text"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_TEXT);
			}
			if (ImGui::MenuItem("Slider"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_SLIDER);
			}
			if (ImGui::MenuItem("Bar"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_BAR);
			}
			if (ImGui::MenuItem("Animated Image"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_ANIMATED_IMAGE);
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

void PanelHierarchy::RightClickSceneNode(GameObject* obj)
{
	if (ImGui::BeginPopupContextItem("##SceneClickNode")) {
		right_click_scene = true;

		if (ImGui::MenuItem("Set Scene Active", nullptr, nullptr, obj->ID != App->objects->scene_active)) {
			App->objects->scene_active = obj->ID;
		}

		if (ImGui::MenuItem("Save Scene")) {
			if (strcmp("Untitled*", obj->GetName()) == 0) {
				App->ui->panel_scene_selector->force_save = obj;
				App->ui->panel_scene_selector->OrganizeSave(PanelSceneSelector::SceneSelectorState::SAVE_AS_NEW);
			}
			else {
				for (auto item = App->objects->current_scenes.begin(); item != App->objects->current_scenes.end(); ++item) {
					if ((*item)->GetID() == obj->ID) {
						App->objects->SaveScene(*item);
						break;
					}
				}
			}
		}
		if (ImGui::MenuItem("Save Scene As")) {
			App->ui->panel_scene_selector->force_save = obj;
			App->ui->panel_scene_selector->OrganizeSave(PanelSceneSelector::SceneSelectorState::SAVE_AS_NEW);
		}
		if (ImGui::BeginMenu("Remove", obj->parent->children.size() > 1)) {
			if (ImGui::MenuItem("Remove Scene")) {
				GameObject::Destroy(obj);
				for (auto item = App->objects->current_scenes.begin(); item != App->objects->current_scenes.end(); ++item) {
					if ((*item)->GetID() == obj->ID) {
						App->objects->current_scenes.erase(item);
						break;
					}
				}
				if (obj->ID == App->objects->scene_active) {
					GameObject* root = App->objects->GetGlobalRoot();
					for (auto item = root->children.begin(); item != root->children.end(); ++item) {
						if ((*item)->ID != obj->ID) {
							App->objects->scene_active = (*item)->ID;
							break;
						}
					}
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}
}
