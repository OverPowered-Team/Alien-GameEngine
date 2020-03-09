#include "ResourceMaterial.h"
#include "Application.h"
#include "ResourceShader.h"
#include "ResourceTexture.h"
#include "imgui/imgui_internal.h"

#include "glew/include/glew.h"
#include "mmgr/mmgr.h"

ResourceMaterial::ResourceMaterial() : Resource()
{
	type = ResourceType::RESOURCE_MATERIAL;

	used_shader = App->resources->default_shader;

	if (used_shader != nullptr)
		used_shader->IncreaseReferences();
	else
		LOG_ENGINE("There was an error. Could not find the default shader");
}

ResourceMaterial::~ResourceMaterial()
{
	if (texture != nullptr)
	{
		RemoveTexture();
	}
}

bool ResourceMaterial::CreateMetaData(const u64& force_id)
{
	if (force_id == 0) {
		ID = App->resources->GetRandomID();
	}
	else {
		ID = force_id;
	}

	// META FILE

	std::string alien_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien").data();

	JSON_Value* alien_value = json_value_init_object();
	JSON_Object* alien_object = json_value_get_object(alien_value);
	json_serialize_to_file_pretty(alien_value, alien_path.data());

	if (alien_value != nullptr && alien_object != nullptr) {
		JSONfilepack* alien = new JSONfilepack(alien_path, alien_object, alien_value);
		alien->StartSave();
		alien->SetString("Meta.ID", std::to_string(ID));
		alien->FinishSave();
		delete alien;
	}

	// LIBRARY FILE 

	meta_data_path = LIBRARY_MATERIALS_FOLDER + std::to_string(ID) + ".material";

	std::string ext;
	App->file_system->SplitFilePath(path.data(), nullptr, nullptr, &ext);
	if (App->StringCmp(ext.data(), "material"))
	{
		App->file_system->Copy(path.data(), meta_data_path.data());
	}
	else
	{
		// ...?
	}

	if(!App->IsQuiting())
		App->resources->AddResource(this);
	return true;
}

bool ResourceMaterial::ReadBaseInfo(const char* assets_file_path)
{
	bool ret = true;

	this->path = assets_file_path;

	// ---------------------------- READ META -----------------------------------
	std::string alien_path = App->file_system->GetPathWithoutExtension(path) + "_meta.alien";

	this->name = App->file_system->GetBaseFileName(path.c_str());

	JSON_Value* value = json_parse_file(alien_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(alien_path, object, value);

		ID = std::stoull(meta->GetString("Meta.ID"));

		delete meta;
	}

	// -------------------------- READ .MATERIAL -----------------------------------

	value = json_parse_file(path.data());
	object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* matFile = new JSONfilepack(alien_path, object, value);

		ReadMaterialValues(matFile);

		delete matFile;
	}

	// -------------------------- READ LIBRARY -------------------------------------
	meta_data_path = LIBRARY_MATERIALS_FOLDER + std::to_string(ID) + ".material";

	if (!App->file_system->Exists(meta_data_path.data())) {
		App->file_system->Copy(path.data(), meta_data_path.data());
		App->resources->AddResource(this);
		return true;
	}

	struct stat fileMeta;
	struct stat fileAssets;

	if (stat(meta_data_path.c_str(), &fileMeta) == 0 && stat(path.c_str(), &fileAssets) == 0) {
		if (fileAssets.st_mtime > fileMeta.st_mtime) {
			remove(meta_data_path.data());
			App->file_system->Copy(path.data(), meta_data_path.data());
		}
	}

	App->resources->AddResource(this);

	return ret;
}

void ResourceMaterial::ReadLibrary(const char* meta_data)
{
	meta_data_path = meta_data;

	JSON_Value* value = json_parse_file(meta_data_path.data());
	JSON_Object* object = json_value_get_object(value);

	ID = std::stoull(App->file_system->GetBaseFileName(meta_data));

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_data_path, object, value);
		ReadMaterialValues(meta);
		delete meta;
	}

	App->resources->AddResource(this);
}

bool ResourceMaterial::DeleteMetaData()
{
	remove(meta_data_path.data());
	return true;
}

void ResourceMaterial::SaveMaterialFiles()
{
	path = std::string(MATERIALS_FOLDER + name + ".material").data();

	remove(path.c_str());

	JSON_Value* alien_value = json_value_init_object();
	JSON_Object* alien_object = json_value_get_object(alien_value);
	json_serialize_to_file_pretty(alien_value, path.data());

	if (alien_value != nullptr && alien_object != nullptr) {
		JSONfilepack* alien = new JSONfilepack(path, alien_object, alien_value);
		SaveMaterialValues(alien);
		delete alien;
	}

	CreateMetaData(ID);
}

void ResourceMaterial::SaveMaterialValues(JSONfilepack* file)
{
	file->StartSave();

	file->SetString("Name", name);
	file->SetFloat4("Color", color);
	file->SetString("ShaderID", std::to_string(used_shader->GetID()));
	file->SetBoolean("HasTexture", texture != nullptr ? true : false);

	if(texture != nullptr)
		file->SetString("TextureID", std::to_string(textureID));
	
	for (uint iter = 0; iter != (uint)TextureType::MAX; ++iter) {
		file->SetString(std::to_string(iter), std::to_string(texturesID[iter]));
	}

	file->FinishSave();
}

void ResourceMaterial::ReadMaterialValues(JSONfilepack* file)
{
	this->name = file->GetString("Name");
	color = file->GetFloat4("Color");

	SetShader((ResourceShader*)App->resources->GetResourceWithID(std::stoull(file->GetString("ShaderID"))));

	if (file->GetBoolean("HasTexture"))
	{
		u64 id = std::stoull(file->GetString("TextureID"));
		SetTexture((ResourceTexture*)App->resources->GetResourceWithID(id));
	}

	for (uint iter = 0; iter != (uint)TextureType::MAX; ++iter) {
		texturesID[iter] = std::stoull(file->GetString(std::to_string(iter)));
	}
}

void ResourceMaterial::ApplyMaterial()
{
	if(texture != nullptr && textureActivated)
		glBindTexture(GL_TEXTURE_2D, texture->id);

	// Bind the actual shader
	used_shader->Bind();

	// Update uniforms
	shaderInputs.standardShaderProperties.diffuse_color = float3(color.x, color.y, color.z);
	used_shader->UpdateUniforms(shaderInputs);
}

void ResourceMaterial::SetTexture(ResourceTexture* tex)
{
	if (tex == nullptr)
	{
		RemoveTexture(); 
		return; 
	}

	if (texture != nullptr)
		texture->DecreaseReferences();

	texture = tex;
	textureID = tex->GetID();
	texture->IncreaseReferences();
}

void ResourceMaterial::RemoveTexture()
{
	if (texture != nullptr)
	{
		texture->DecreaseReferences();
		texture = nullptr;
		textureID = 0;
	}
}

bool ResourceMaterial::HasTexture() const
{
	return texture != nullptr;
}

void ResourceMaterial::DisplayMaterialOnInspector()
{
	if (ImGui::CollapsingHeader(GetName(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SameLine();
		MaterialHeader();

		if (this == App->resources->default_material)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		ShaderSelectionHeader();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ShaderInputsSegment();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		TexturesSegment();

		if (this == App->resources->default_material)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
	}
}

void ResourceMaterial::MaterialHeader()
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(std::string("Material References: " + std::to_string(references)).c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}


void ResourceMaterial::ShaderSelectionHeader()
{
	std::vector<ResourceShader*> shadersList;
	App->resources->GetShaders(shadersList);
	static const char* selectedShader = used_shader->GetName();
	if (ImGui::BeginCombo("Selected Shader", selectedShader))
	{
		for (std::vector<ResourceShader*>::iterator iter = shadersList.begin(); iter != shadersList.end(); iter++)
		{
			bool is_selected = (App->StringCmp(selectedShader, (*iter)->GetName()));
			if (ImGui::Selectable((*iter)->GetName(), is_selected))
			{
				selectedShader = (*iter)->GetName();
				ResourceShader* newShader = App->resources->GetShaderByName(selectedShader);

				if (newShader != nullptr)
				{
					SetShader(newShader);
					LOG_ENGINE("Selected Shader %s\n", used_shader->GetName());
				}
				else
					is_selected = false;

			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void ResourceMaterial::ShaderInputsSegment()
{
	switch (used_shader->GetShaderType())
	{
	case SHADER_TEMPLATE::DEFAULT: {//difusse
		//ImGui::ColorEdit3("Albedo", shaderInputs.standardShaderProperties.diffuse_color.ptr(), ImGuiColorEditFlags_Float);
		ImGui::ColorEdit3("Albedo", color.ptr(), ImGuiColorEditFlags_Float);
		break; }
	case SHADER_TEMPLATE::WAVE: {//wave
								
		ImGui::Spacing();

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "velocity", (unsigned int)type); ImGui::SameLine();
		ImGui::InputFloat("## ", &shaderInputs.waveShaderProperties.mult_time, 0, 0, 2);

		ImGui::Spacing();

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "amplitude", (unsigned int)type); ImGui::SameLine();
		ImGui::InputFloat("##", &shaderInputs.waveShaderProperties.amplitude, 0, 0, 2);

		break; }

	case SHADER_TEMPLATE::ILUMINATED: {
		ImGui::ColorEdit3("Albedo", shaderInputs.iluminatedShaderProperties.object_color.ptr(), ImGuiColorEditFlags_Float);
		break; }

	default:
		LOG_ENGINE("We currently don't support editing this type of uniform...");
		break;
	}
}

void ResourceMaterial::TexturesSegment()
{
	//static ResourceTexture* selected_texture = nullptr;
	if (texture != nullptr)
	{	
		ImGui::Text("Texture Information: %s", std::to_string(texture->GetID()).c_str());

		//ImGui::SameLine(140, 15);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
		if (ImGui::Button("Delete", { 60,20 })) {
			//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			RemoveTexture();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			return;
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		static bool check;
		check = textureActivated;
		if (ImGui::Checkbox("Texture Active", &check)) {
			//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			textureActivated = check;
		}
		ImGui::SameLine(220, 15);
		ImGui::Spacing();
		ImGui::Text("Texture References: %i", texture->references);
		ImGui::Text("Texture Size:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->width);
		ImGui::SameLine(); ImGui::Text("x"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->height);
		ImGui::Text("Path:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%s", texture->GetAssetsPath());

		ImGui::Image((ImTextureID)texture->id, { ImGui::GetWindowWidth() ,ImGui::GetWindowWidth() });
		ImGui::Spacing();
	}
	//else {
	//	ImGui::SameLine(220, 15);
	//	if (ImGui::Button("Add Texture", { 120,20 })) {
	//		/*change_texture_menu = true;
	//		selected_texture = texture;*/
	//	}
	//}

	 //Textures window preview 

	/*if (change_texture_menu) {
		ImGui::OpenPopup("Textures Loaded");
		ImGui::SetNextWindowSize({ 522,570 });
		if (ImGui::BeginPopupModal("Textures Loaded", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImGui::Spacing();
			ImGui::NewLine();
			ImGui::SameLine(190);
			ImGui::Text("Texture Selected");
			ImGui::Text("");
			ImGui::SameLine(170);
			if (selected_texture != nullptr) {
				ImGui::Image((ImTextureID)selected_texture->id, { 150,150 });
				ImGui::Spacing();
				ImGui::Text("");
				ImGui::SameLine(150);
				ImGui::Text("Texture Size:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->width);
				ImGui::SameLine(); ImGui::Text("x"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->height);
				ImGui::Text("");
				ImGui::SameLine(112);
				ImGui::Text("Path:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%s", selected_texture->GetAssetsPath());
			}
			ImGui::Spacing();

			if (ImGui::BeginChild("##TexturesSelectorChild", { 492,285 }, true, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
				ImGui::Columns(3, 0, false);
				ImGui::SetColumnWidth(0, 156);
				ImGui::SetColumnWidth(1, 156);
				ImGui::SetColumnWidth(2, 156);

				std::vector<Resource*>::iterator item = App->resources->resources.begin();
				for (; item != App->resources->resources.end(); ++item) {
					if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {
						ImGui::ImageButton((ImTextureID)static_cast<ResourceTexture*>(*item)->id, { 140,140 });
						if (ImGui::IsItemClicked()) {
							selected_texture = static_cast<ResourceTexture*>(*item);
						}
						ImGui::NextColumn();
					}
				}

				ImGui::EndChild();
			}
			ImGui::Spacing();
			ImGui::Text("");
			ImGui::SameLine(377);
			if (ImGui::Button("Apply", { 120,20 })) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				texture = selected_texture;
				selected_texture = nullptr;
				change_texture_menu = false;
			}
			ImGui::SameLine(237);
			if (ImGui::Button("Cancel", { 120,20 })) {
				selected_texture = nullptr;
				change_texture_menu = false;
			}
			ImGui::EndPopup();
		}
	}*/
}

void ResourceMaterial::SetShader(ResourceShader* newShader)
{
	if (newShader == nullptr || newShader == used_shader)
		return;

	used_shader->DecreaseReferences();
	used_shader = newShader;
	used_shader->IncreaseReferences();
}


