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

void ComponentBar::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("X", x);
	to_save->SetNumber("Y", y);
	to_save->SetNumber("Width", size.x);
	to_save->SetNumber("Height", size.y);

	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("UIType", (int)ui_type);
	to_save->SetString("TextureID", (texture != nullptr) ? std::to_string(texture->GetID()) : "0");
	to_save->SetColor("Color", current_color);

	to_save->SetNumber("MaxValue", maxValue);
	to_save->SetNumber("Value", value);
	to_save->SetNumber("BarID", bar->ID);
}

void ComponentBar::LoadComponent(JSONArraypack* to_load)
{
	x = to_load->GetNumber("X");
	y = to_load->GetNumber("Y");
	size = { (float)to_load->GetNumber("Width"), (float)to_load->GetNumber("Height") };
	UpdateVertex();

	enabled = to_load->GetBoolean("Enabled");
	current_color = to_load->GetColor("Color");
	maxValue = to_load->GetNumber("MaxValue");
	value = to_load->GetNumber("Value");

	u64 textureID = std::stoull(to_load->GetString("TextureID"));
	if (textureID != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
		if (tex != nullptr) {
			SetTexture(tex);
		}
	}
	GameObject* p = game_object_attached->parent;
	bool changed = true;
	while (changed) {
		if (p != nullptr) {
			ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
			if (canvas != nullptr) {
				SetCanvas(canvas);
				changed = false;
			}
			p = p->parent;
		}
		else {
			changed = false;
			SetCanvas(nullptr);
		}
	}
}
