#include "PanelTerrainEditor.h"
#include "ShortCutManager.h"


PanelTerrainEditor::PanelTerrainEditor(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{

	shortcut = App->shortcut_manager->AddShortCut("Panel Terrain Editor", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
	
}



PanelTerrainEditor::~PanelTerrainEditor()
{



}



void PanelTerrainEditor::PanelLogic()
{

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse  | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;


	if (ImGui::Begin("Terrain Editor", &enabled, flags))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Puta vida :(");

		if (ImGui::MenuItem("Open terrain tools"))
		{
			
		}
		if (ImGui::MenuItem("Heightmap"))
		{

		}
		if (ImGui::MenuItem("Textures"))
		{

		}

	}
	ImGui::End();

}
