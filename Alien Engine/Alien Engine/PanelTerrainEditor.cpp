#include "PanelTerrainEditor.h"
#include "ShortCutManager.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "FileNode.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

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

		if (ImGui::TreeNodeEx("HEIGHTMAP", ImGuiTreeNodeFlags_Framed))
		{

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
			ImGui::Spacing();
			ImGui::Spacing();
			if(ImGui::Button("Create Blur") && heigthmapTexture != nullptr)
			{
				std::string lib_path = heigthmapTexture->GetLibraryPath();
				GenerateHeightMap(lib_path.data(), 10);
			}

		
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "HeightMap Texture:");

			if (heigthmapTexture != nullptr)
			{
				float2 size = GetHeightMapSize();

				float maxSize = max(size.x, size.y);
				if (maxSize > 500)
				{
					float scale = 500.0f / maxSize;
					size.x *= scale;
					size.y *= scale;
				}

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Image((void*)heigthmapTexture->id, ImVec2(size.x, size.y));
			}
			else
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "No Texture loaded");
				ImGui::Spacing();
			}


			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "HeightMap Texture Blurred:");
		
			int hp = GetHeightMapID();
			if (hp != 0)
			{
				float2 size = GetHeightMapSize();

				float maxSize = max(size.x, size.y);
				if (maxSize > 500)
				{
					float scale = 500.0f / maxSize;
					size.x *= scale;
					size.y *= scale;
				}

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Image((void*)hp, ImVec2(size.x, size.y));
			}
			else
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "No Texture loaded");
				ImGui::Spacing();
			}


			ImGui::TreePop();
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
	heigthmapTexture->IncreaseReferences();
}

uint PanelTerrainEditor::GetHeightMapID()
{
	return heightmap_id;
}

float2 PanelTerrainEditor::GetHeightMapSize()
{
	return float2(m_Width, m_Height);
}

void PanelTerrainEditor::GenerateHeightMap(const char* path, int iterations)
{
	SetPixelData(path);
	GaussianBlur(iterations);
	GenerateHeightMapBuffer();
}

void PanelTerrainEditor::SetPixelData(const char* path)
{
	ILuint new_image_id = 0;
	ilGenImages(1, &new_image_id);
	ilBindImage(new_image_id);

	ilutRenderer(ILUT_OPENGL);

	if (ilutGLLoadImage((char*)path))
	{

		m_Width = ilGetInteger(IL_IMAGE_WIDTH);
		m_Height = ilGetInteger(IL_IMAGE_HEIGHT);


		m_data.resize(m_Width * m_Height, float3(0.f));

		ilCopyPixels(0, 0, 0, m_Width, m_Height, 1, IL_RGB, IL_FLOAT, m_data.data());

		ILenum err = ilGetError();
		if (err != IL_NO_ERROR)
		{
			const char* err_str = iluErrorString(err);
			std::cout << "Error copy pixels RGB" << std::endl;
		}


		ilBindImage(0);
		ilDeleteImages(1, &new_image_id);
	}


}

void PanelTerrainEditor::GenerateHeightMapBuffer()
{

	if (m_data.data())
	{
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &heightmap_id);

		glBindTexture(GL_TEXTURE_2D, heightmap_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, m_data.data());

		
	}



}

void PanelTerrainEditor::GaussianBlur(int iterations)
{
	ILuint handle;
	ilGenImages(1, &handle);

	ilBindImage(handle);

	ilTexImage(m_Width, m_Height, 1, 3, IL_RGB, IL_FLOAT, m_data.data());
	iluBlurGaussian(iterations);

	ILenum err = ilGetError();
	if (err != IL_NO_ERROR)
	{
		const char* err_str = iluErrorString(err);
		std::cout << "Error gaussian blur: " << err_str << std::endl;
	}

	ilCopyPixels(0, 0, 0, m_Width, m_Height, 1, IL_RGB, IL_FLOAT, m_data.data());
	err = ilGetError();

	if (err != IL_NO_ERROR)
	{
		const char* err_str = iluErrorString(err);
		std::cout << "Error copy pixels" << std::endl;

	}
}
