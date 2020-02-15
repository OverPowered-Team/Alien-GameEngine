#include "ComponentImage.h"
#include "imgui/imgui.h"
#include "ComponentUI.h"
#include "ReturnZ.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "PanelProject.h"

ComponentImage::ComponentImage(GameObject* obj) : ComponentUI(obj)
{
	ui_type = ComponentType::UI_IMAGE;
	width = 10;
	height = 10;
}

bool ComponentImage::DrawInspector()
{
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Image", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Image");

		ImGui::Spacing();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Texture");

		ImGui::SameLine(85);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

		ImGui::Button((texture == nullptr) ? "NULL" : std::string(texture->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

		if (ImGui::IsItemClicked() && texture != nullptr) {
			App->ui->panel_project->SelectFile(texture->GetAssetsPath(), App->resources->assets);
		}

		ImGui::PopStyleColor(3);
		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
			if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
				FileNode* node = *(FileNode**)payload->Data;
				if (node != nullptr && node->type == FileDropType::TEXTURE) {
					std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
					path += "_meta.alien";
					u64 ID = App->resources->GetIDFromAlienPath(path.data());
					if (ID != 0) {
						ResourceTexture* texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
						if (texture != nullptr) {
							SetTexture(texture);
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (texture != nullptr) {
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X") && texture != nullptr) {
				ClearTexture();
			}
			ImGui::PopStyleColor(3);
		}
		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Color");
		ImGui::SameLine(85);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColor", &current_color, ImGuiColorEditFlags_Float)) {

		}
		ImGui::Spacing();


		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Image");
	}

	return true;
}

void ComponentImage::ClearTexture()
{
	if (texture != nullptr) {
		texture->DecreaseReferences();
		texture = nullptr;

		width = 10;
		height = 10;

		glDeleteBuffers(1, &verticesID);
		glDeleteBuffers(1, &uvID);

		vertices[0] = { -1,1,0 };
		vertices[1] = { -1,-1,0 };
		vertices[2] = { 1,-1,0 };
		vertices[3] = { 1,1,0 };

		uv[0] = { -1,-1 };
		uv[1] = { -1,0 };
		uv[2] = { 0,0 };
		uv[3] = { 0,-1 };

		glGenBuffers(1, &verticesID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 3, vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &uvID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uvID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 2, uv, GL_STATIC_DRAW);
	}
}

void ComponentImage::SetTexture(ResourceTexture* tex)
{
	if (tex != nullptr && tex != texture) {
		tex->IncreaseReferences();
		if (texture != nullptr) {
			texture->DecreaseReferences();
		}
		texture = tex;

		width = (float)tex->width / 100;
		height = (float)tex->height / 100;

		glDeleteBuffers(1, &verticesID);

		float halfWidth = width * 0.5F;
		float halfHeight = height * 0.5F;

		vertices[0] = { -halfWidth, halfHeight, 0 };
		vertices[1] = { -halfWidth, -halfHeight, 0 };
		vertices[2] = { halfWidth, -halfHeight, 0 };
		vertices[3] = { halfWidth, halfHeight, 0 };

		glGenBuffers(1, &verticesID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 3, vertices, GL_STATIC_DRAW);
	}
}
