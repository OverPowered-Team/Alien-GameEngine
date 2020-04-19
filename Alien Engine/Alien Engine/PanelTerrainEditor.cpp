#include "PanelTerrainEditor.h"
#include "ShortCutManager.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "FileNode.h"
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

	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_HorizontalScrollbar;

	if (ImGui::Begin("Terrain Editor", &enabled, flags))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Puta vida :(");


		ImGui::Text("HeightMap");
		

		if(heigthmapTexture != nullptr)
			ImGui::ImageButton((ImTextureID)heigthmapTexture->id, ImVec2(30, 30));
		else
			ImGui::ImageButton((ImTextureID)0, ImVec2(30, 30));

		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
			if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
				FileNode* node = *(FileNode**)payload->Data;
				if (node != nullptr && node->type == FileDropType::TEXTURE) {
					std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
					path += "_meta.alien";
					u64 ID = App->resources->GetIDFromAlienPath(path.data());
					if (ID != 0) {
						ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(ID);
						if (tex != nullptr) {
							SetHeigthMapTexture(tex);
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
		
		if (ImGui::RadioButton("###", false))
		{
		}
		

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

void PanelTerrainEditor::SetHeigthMapTexture(ResourceTexture* tex)
{
	heigthmapTexture = tex;
}
