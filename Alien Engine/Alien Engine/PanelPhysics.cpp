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
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(300, 900));

	if (ImGui::Begin(panel_name.data(), &enabled))
	{
		float3 current_gravity = App->physics->gravity;

		ImGui::Spacing();
		ImGui::Title("Gravity");	if (ImGui::DragFloat3("##gravity", current_gravity.ptr(), 0.05f)) { App->physics->SetGravity(current_gravity); }
		ImGui::Title("Draw All");	ImGui::Checkbox("##debug_all_physics", &App->physics->debug_physics);
	}

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopID();
}
