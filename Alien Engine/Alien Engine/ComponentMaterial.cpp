#include <fstream>
#include "ComponentMaterial.h"
#include "glew/include/glew.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "ComponentMesh.h"
#include "Application.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "ReturnZ.h"

ComponentMaterial::ComponentMaterial(GameObject* attach) : Component(attach)
{
	type = ComponentType::MATERIAL;

	u64 id_s = App->resources->GetIDFromAlienPath(SHADERS_FOLDER "default_meta.alien"); // needs fix. meta is not created too...
	used_shader = (ResourceShader*)App->resources->GetResourceWithID(id_s);
	used_shader->IncreaseReferences();
	file_to_edit = used_shader->path;
}

ComponentMaterial::~ComponentMaterial()
{
	if (texture != nullptr)
		texture->DecreaseReferences();
	if (used_shader != nullptr)
		used_shader->DecreaseReferences();
}

void ComponentMaterial::BindTexture()
{
	ComponentMesh* mesh = game_object_attached->GetComponent<ComponentMesh>();
	if (texture != nullptr && texture->id > 0 && texture_activated && mesh != nullptr && mesh->mesh != nullptr) {
		// enable textures
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}
	glColor4f(color.r, color.g, color.b, color.a);
	
}

bool ComponentMaterial::DrawInspector()
{
	static bool en;
	ImGui::PushID(this);
	en = enabled;
	if (ImGui::Checkbox("##CmpActive", &en)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = en;
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Material", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Material");
		if (texture != nullptr) {
			ImGui::Spacing();
			ImGui::Text("Texture References: %i", texture->references);
		}
		ImGui::Spacing();
		static bool set_Z = true;
		ImGui::Spacing();
		static Color col;
		col = color;
		if (ImGui::ColorEdit3("Material Color", &col, ImGuiColorEditFlags_Float)) {
			if (set_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_Z = false;
			color = col;
		}
		else if (!set_Z && ImGui::IsMouseReleased(0)) {
			set_Z = true;
		}

		if (ImGui::SliderFloat("Alpha", &col.a, 0.0F, 1.0F)) {
			if (set_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_Z = false;
			color.a = col.a;
		}
		else if (!set_Z && ImGui::IsMouseReleased(0)) {
			set_Z = true;
		}

		ImGui::Spacing();

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::Text("Texture Information");

		static ResourceTexture* selected_texture = nullptr;
		if (texture != nullptr)
		{
			ImGui::SameLine(220, 15);
			if (ImGui::Button("Change Texture", { 120,20 })) {
				/*change_texture_menu = true;
				selected_texture = texture;*/
			}

			ImGui::SameLine(140, 15);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("Delete", { 60,20 })) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				texture = nullptr;
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				return true;
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

			static bool check;
			check = texture_activated;
			if (ImGui::Checkbox("Texture Active", &check)) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				texture_activated = check;
			}

			ImGui::Text("Texture Size:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->width);
			ImGui::SameLine(); ImGui::Text("x"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->height);
			ImGui::Text("Path:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%s", texture->GetAssetsPath());

			ImGui::Image((ImTextureID)texture->id, { ImGui::GetWindowWidth() ,ImGui::GetWindowWidth() });
			ImGui::Spacing();
		}
		else {
			ImGui::SameLine(220, 15);
			if (ImGui::Button("Add Texture", { 120,20 })) {
				/*change_texture_menu = true;
				selected_texture = texture;*/
			}
		}

		if (change_texture_menu) {
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
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		/* Shaders */
		if (used_shader != nullptr)
		{
			ImGui::Text("Current shader: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), used_shader->path.c_str());
			if (ImGui::Button("Select Shader"))
			{
				select_shader = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Edit shader"))
			{
				{
					std::ifstream t(file_to_edit.c_str());
					if (t.good())
					{
						std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
						shader_editor.SetText(str);
					}
				}

				show_shader_text_editor = true;
			}

			if (ImGui::Button("Compile shader")) // TODO: Compile automatically when we save and show error
			{
				used_shader->ParseAndCreateShader();
			}

			ImGui::Separator();

			/* Set shader unifroms from Inspector */
			ImGui::Text("--Uniforms--");
			
			GLint uniform_count = 0;
			const GLsizei u_buff_size = 24;
			glGetProgramiv(used_shader->renderer_id, GL_ACTIVE_UNIFORMS, &uniform_count);
			for (GLuint i = 0; i < uniform_count; ++i)
			{
				GLsizei length;
				GLint size;
				GLenum type;
				GLchar name[u_buff_size];
				glGetActiveUniform(used_shader->renderer_id, i, (GLsizei)20, &length, &size, &type, name);

				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s(%u)", (const char*)name, (unsigned int)type);
			}

			ImGui::Separator();

			// Can select desired shader in the shaders folder
			if (select_shader)
			{
				if (ImGui::Begin("Select Shader", &select_shader))
				{
					if (ImGui::Button("Close"))
					{
						select_shader = false;
					}

					std::vector<ResourceShader*> shaders;
					App->resources->GetShaders(shaders);
					for (auto i = shaders.begin(); i != shaders.end(); ++i) {
						if (ImGui::Button((*i)->GetName()))
						{
							u64 id_s = App->resources->GetIDFromAlienPath(std::string(std::string((*i)->GetAssetsPath()) + "_meta.alien").data());
							if (used_shader != nullptr) {
								used_shader->DecreaseReferences();
							}
							used_shader = (ResourceShader*)App->resources->GetResourceWithID(id_s);
							if (used_shader != nullptr) {
								used_shader->IncreaseReferences();
							}
							file_to_edit = used_shader->path; // must test if it edits on library too in this engine
						}
					}

					ImGui::End();
				}
			}

			if (show_shader_text_editor)
			{
				ShowShaderTextEditor();
			}
		}
	}
	else
		RightClickMenu("Material");

	return true;
}

void ComponentMaterial::Reset()
{
	color = { 1,1,1,1 };
	if (texture != nullptr) {
		texture->DecreaseReferences();
	}
	texture = nullptr;
}

void ComponentMaterial::SetComponent(Component* component)
{
	/*if (component->GetType() == type) {

		ComponentMaterial* material = (ComponentMaterial*)component;
		if (texture != nullptr) {
			texture->DecreaseReferences();
		}
		texture = material->texture;
		if (texture != nullptr) {
			texture->IncreaseReferences();
		}

		color = material->color;
	}*/
	if (component->GetType() == type) {

		ComponentMaterial* material = (ComponentMaterial*)component;
		if (texture != nullptr) {
			texture->DecreaseReferences();
		}
		texture = material->texture;
		if (texture != nullptr) {
			texture->IncreaseReferences();
		}
		if (used_shader != nullptr) {
			used_shader->DecreaseReferences();
		}
		used_shader = material->used_shader;
		if (used_shader != nullptr) {
			used_shader->IncreaseReferences();
		}

		color = material->color;
	}
}

void ComponentMaterial::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetColor("Color", color);
	to_save->SetBoolean("TextureEnabled", texture_activated);
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetBoolean("HasTexture", (texture != nullptr) ? true : false);
	if (texture != nullptr) {
		to_save->SetString("TextureID", std::to_string(texture->GetID()));
	}
	to_save->SetBoolean("Enabled", enabled);
}

void ComponentMaterial::LoadComponent(JSONArraypack* to_load)
{
	color = to_load->GetColor("Color");
	texture_activated = to_load->GetBoolean("TextureEnabled");
	enabled = to_load->GetBoolean("Enabled");
	if (to_load->GetBoolean("HasTexture")) {
		u64 ID = std::stoull(to_load->GetString("TextureID"));
		texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
		if (texture != nullptr)
			texture->IncreaseReferences();
	}
	ID = std::stoull(to_load->GetString("ID"));
}

void ComponentMaterial::Clone(Component* clone)
{
	/*clone->enabled = enabled;
	clone->not_destroy = not_destroy;
	ComponentMaterial* mat = (ComponentMaterial*)clone;
	mat->color = color;
	mat->texture = texture;
	if (texture != nullptr) {
		++texture->references;
	}
	mat->texture_activated = texture_activated;*/

	clone->enabled = enabled;
	clone->not_destroy = not_destroy;
	ComponentMaterial* mat = (ComponentMaterial*)clone;
	mat->color = color;
	mat->texture = texture;
	mat->used_shader = used_shader;
	if (texture != nullptr) {
		++texture->references;
	}
	if (used_shader != nullptr) {
		++used_shader->references;
	}
	mat->texture_activated = texture_activated;
}

void ComponentMaterial::SetTexture(ResourceTexture* tex)
{
	if (texture != nullptr) {
		texture->DecreaseReferences();
	}
	texture = tex;
	if (texture != nullptr) {
		texture->IncreaseReferences();
	}
}

const ResourceTexture* ComponentMaterial::GetTexture() const
{
	return texture;
}

void ComponentMaterial::ShowShaderTextEditor()
{
	auto cpos = shader_editor.GetCursorPosition();
	ImGui::SetNextWindowPosCenter(ImGuiCond_FirstUseEver);
	ImGui::Begin("Shader Editor", &show_shader_text_editor, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				auto textToSave = shader_editor.GetText();

				// Save text assets folder
				App->file_system->Save(file_to_edit.c_str(), textToSave.c_str(), textToSave.size());

				// Save text library folder
				App->file_system->Save(used_shader->GetLibraryPath(), textToSave.c_str(), textToSave.size());
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = shader_editor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				shader_editor.SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && shader_editor.CanUndo()))
				shader_editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && shader_editor.CanRedo()))
				shader_editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, shader_editor.HasSelection()))
				shader_editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && shader_editor.HasSelection()))
				shader_editor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && shader_editor.HasSelection()))
				shader_editor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				shader_editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				shader_editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(shader_editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Dark palette"))
				shader_editor.SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light palette"))
				shader_editor.SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro blue palette"))
				shader_editor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, shader_editor.GetTotalLines(),
		shader_editor.IsOverwrite() ? "Ovr" : "Ins",
		shader_editor.CanUndo() ? "*" : " ",
		shader_editor.GetLanguageDefinition().mName.c_str(), file_to_edit.c_str());

	shader_editor.Render("TextEditor");

	ImGui::End();
}
