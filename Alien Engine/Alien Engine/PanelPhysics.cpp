#include "PanelPhysics.h"
#include "ShortCutManager.h"
#include "ModulePhysics.h"
#include "Optick/include/optick.h"

PanelPhysics::PanelPhysics(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra) :
	Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Panel Physics", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
	enabled = false;
}

PanelPhysics::~PanelPhysics()
{
}

void PanelPhysics::PanelLogic()
{
	OPTICK_EVENT();

	ImGui::PushID(this);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(450, 900));

	if (ImGui::Begin(panel_name.data(), &enabled))
	{
		float3 current_gravity = App->physics->gravity;

		ImGui::Spacing();
		ImGui::Title("Gravity");	if (ImGui::DragFloat3("##gravity", current_gravity.ptr(), 0.05f)) { App->physics->SetGravity(current_gravity); }
		ImGui::Title("Draw All");	ImGui::Checkbox("##debug_all_physics", &App->physics->debug_physics);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Layer");


		if (ImGui::BeginComboEx(std::string("##layers").c_str(), std::string(" " + App->physics->layers.at(current_layer)).c_str(), 200, ImGuiComboFlags_NoArrowButton))
		{
			for (int n = 0; n < App->physics->layers.size(); ++n)
			{
				bool is_selected = (current_layer == n);

				if (ImGui::Selectable(std::string(" " + App->physics->layers.at(n)).c_str(), is_selected))
				{
					current_layer = n;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Title("");

		if (ImGui::Button("+")) open_add_popup = true;

		if (current_layer != 0)
		{
			ImGui::SameLine();
			if (ImGui::Button("-"))	 open_remove_popup = true;
		}


		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::NewLine();

		int size = App->physics->layers.size();
		int separation = 150;

		for (int i = size - 1 ; i >= 0; --i)
		{
			ImGui::SameLine(separation);
			ImGui::Text(std::string(std::to_string(i) + ".").c_str());
			separation += 24;
		}

		for (int i = 0; i < size; ++i)
		{
			ImGui::Title(std::string(std::to_string(i) + ". " + App->physics->layers.at(i)).c_str());
			separation = 150;
			
			for (int j = size - 1 - i; j >= 0; --j)
			{
				ImGui::PushID(&App->physics->layers_table[i][j]);
				ImGui::SameLine(separation);
				ImGui::Checkbox("##CHECK", &App->physics->layers_table[i][j]);
				separation += 24;
				ImGui::PopID();
			}
		}

	}

	ImGui::End();
	ImGui::PopStyleVar();

	PopUpAddLayer();
	PopUpRemoveLayer();

	ImGui::PopID();

}

void PanelPhysics::PopUpAddLayer()
{
	static char layer_name[40] = "";

	if (open_add_popup) 
	{
		ImGui::OpenPopup("  Add Layer");
		ImGui::SetNextWindowSize({ 410,90 });

		if (ImGui::BeginPopupModal("  Add Layer", &open_add_popup, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Title("Name");
			ImGui::InputText("##name_layer", layer_name, 40);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::SetCursorPosX(160);
			if (ImGui::Button("Acept")) {
				open_add_popup = false;
				App->physics->AddCollisionLayer(std::string(layer_name));
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				open_add_popup = false;
			}

			ImGui::EndPopup();
		}
	}
}

void PanelPhysics::PopUpRemoveLayer()
{
	if (open_remove_popup)
	{
		ImGui::OpenPopup("  Add Layer");
		ImGui::SetNextWindowSize({ 410,90 });

		if (ImGui::BeginPopupModal("  Add Layer", &open_remove_popup, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text(("  Do you want to delete \"" + App->physics->layers[current_layer] + "\" ?").data());

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::SetCursorPosX(160);
			if (ImGui::Button("Acept")) {
				App->physics->RemoveCollisionLayer(App->physics->layers[current_layer]);
				current_layer = 0;
				open_remove_popup = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				open_remove_popup = false;
			}

			ImGui::EndPopup();
		}
	}
}


