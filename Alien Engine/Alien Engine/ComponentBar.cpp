#include "Application.h"
#include "ModuleUI.h"
#include "PanelProject.h"
#include "PanelGame.h"
#include "ComponentCamera.h"
#include "ComponentImage.h"
#include "ComponentTransform.h"
#include "ComponentBar.h"
#include "ResourceTexture.h"
#include "ReturnZ.h"
#include "imgui/imgui.h"

ComponentBar::ComponentBar(GameObject* obj):ComponentUI(obj)
{
	ChangeVertex(20, 5);

	//---------------------------------------------------------
	bar = new GameObject(game_object_attached);
	bar->SetName("Bar");
	bar->AddComponent(new ComponentTransform(bar, { 0,0,0 }, Quat::identity(), { 0.85, 0.85f, 0.85f }));
	ComponentImage* comp = new ComponentImage(bar);
	comp->ChangeVertex(19.4, 5);
	comp->ChangePosition(0.4, -0.5);
	comp->UpdateBar(value/maxValue);
	bar->AddComponent(comp);

	ui_type = ComponentType::UI_BAR;
}

bool ComponentBar::DrawInspector()
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

	if (ImGui::CollapsingHeader("Bar", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Bar");

		ImGui::Spacing();

		ImGui::PushID(this);
		ImGui::Text("Position:	"); ImGui::SameLine(); ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("X", &x, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static))
			UpdateVertex();
		ImGui::SameLine(); ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("Y", &y, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static))
			UpdateVertex();
		ImGui::Text("Size:		"); ImGui::SameLine(); ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("W", &size.x, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static))
			UpdateVertex();
		ImGui::SameLine(); ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("H", &size.y, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static))
			UpdateVertex();

		ImGui::PopID();

		ImGui::Text("Value:		"); ImGui::SameLine();
		if (ImGui::DragFloat("##Value", &value, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static))
		{
			if (value > maxValue) value = maxValue;
			if (value < 0) value = 0;
			bar->GetComponent<ComponentImage>()->UpdateBar(value / maxValue);
		}
			
		ImGui::Text("Max value:	"); ImGui::SameLine();
		if (ImGui::DragFloat("##MaxValue", &maxValue, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static))
		{
			if (maxValue < value ) maxValue = value;
			bar->GetComponent<ComponentImage>()->UpdateBar(value / maxValue);
		}

		ImGui::Text("Texture");

		ImGui::SameLine(120);
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
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Bar");
	}

	return true;
}
