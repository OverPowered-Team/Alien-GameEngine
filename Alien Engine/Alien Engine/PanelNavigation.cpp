#include "PanelNavigation.h"
#include "ShortCutManager.h"
//#include "ModuleNavigation.h"
#include "Optick/include/optick.h"
#include "ModuleObjects.h"


PanelNavigation::PanelNavigation(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra) :
	Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Panel Navigation", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
	enabled = false;
}

PanelNavigation::~PanelNavigation()
{
}

void PanelNavigation::PanelLogic()
{
	OPTICK_EVENT();

	// -----------------------------------------------
	std::list<GameObject*> gos = App->objects->GetSelectedObjects();
	std::vector<GameObject*> mesh_gos;

	for (std::list<GameObject*>::iterator it = gos.begin(); it != gos.end(); ++it) {
		if ((*it)->GetComponent(ComponentType::MESH) != nullptr) { // TODO: needs to check mesh in children?
			mesh_gos.push_back((*it));
		}
	}

	ImGui::Begin(panel_name.data(), &enabled);
	ImGui::Title("Panel Navigation");
	ImGui::Spacing();
	ImGui::Separator();
	// ----------------------------------------------
	// Used to know when to draw navigation meshes and other recast data on scene panel
	ImDrawList* list = ImGui::GetWindowDrawList();
	if (list != nullptr && list->IdxBuffer.size() > 0) {
		panel_nav_rendered = true;
	}
	else {
		panel_nav_rendered = false;
	}
	// -----------------------------------------------
	uint num_go_valids = mesh_gos.size();
	ImGui::Text("selected %i gameobject/s", gos.size());
	ImGui::Text("with %i meshes", num_go_valids);

	ImGui::Separator();

	if (num_go_valids == 0)
	{
		ImGui::Text("Select any gameobject with mesh from the scene");
	}
	else
	{
		ImGui::Text("%s", mesh_gos.front()->GetName());
		if (num_go_valids > 1)
		{
			ImGui::SameLine();
			ImGui::Text(num_go_valids > 2 ? "and %i others" : "and %i other", num_go_valids - 1);
		}

		GameObject* first_selected = mesh_gos.front();
		if (ImGui::Checkbox("Navigation Static", &first_selected->nav_data.nav_static))
		{
			// change all valids gameobjects too at once
			for (uint i = 0; i < num_go_valids; ++i)
				mesh_gos[i]->nav_data.nav_static = first_selected->nav_data.nav_static;
		}
	}

	
	if (ImGui::Button("Bake", ImVec2(100, 20))) {
		LOG_ENGINE("TODO: send mesh gos list to Module Navigation");
	}

	if (enabled != true)
		OnPanelDesactive();

	ImGui::End();
}

void PanelNavigation::OnPanelDesactive() // TODO: this call doesn't work if the window is NOT docked and is closed by the top right X icon
{
	panel_nav_rendered = false;
}