#include "ComponentCheckbox.h"
#include "ComponentCanvas.h"
#include "ComponentImage.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "ComponentUI.h"
#include "ReturnZ.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "PanelProject.h"
#include "ComponentTransform.h"

ComponentCheckbox::ComponentCheckbox(GameObject* obj) :ComponentUI(obj)
{
	ui_type = ComponentType::UI_CHECKBOX;

	//---------------------------------------------------------
	tick = new GameObject(game_object_attached);
	tick->SetName("tick");
	tick->AddComponent(new ComponentTransform(tick, { 0,0,0 }, Quat::identity(), { 0.5f,0.5f,0.5f }));
	ComponentImage* comp = new ComponentImage(tick);
	tick->AddComponent(comp);

	//---------------------------------------------------------
	cross = new GameObject(game_object_attached);
	cross->SetName("cross");
	cross->AddComponent(new ComponentTransform(cross, { 0,0,0 }, Quat::identity(), { 0.5f,0.5f,0.5f }));
	ComponentImage* comp2 = new ComponentImage(cross);
	cross->AddComponent(comp2);
	

}

bool ComponentCheckbox::DrawInspector()
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

	if (ImGui::CollapsingHeader("Checkbox", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Checkbox");

		ImGui::Spacing();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
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
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Idle Color");
		ImGui::SameLine(120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColorIdle", &idle_color, ImGuiColorEditFlags_Float)) {
			current_color = idle_color;
		}

		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Hover Color");
		ImGui::SameLine(120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColorHover", &hover_color, ImGuiColorEditFlags_Float)) {

		}

		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Click Color");
		ImGui::SameLine(120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColorClick", &clicked_color, ImGuiColorEditFlags_Float)) {

		}
		ImGui::Spacing();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Pressed Color");
		ImGui::SameLine(120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColorPressed", &pressed_color, ImGuiColorEditFlags_Float)) {

		}
		ImGui::Spacing();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Disabled Color");
		ImGui::SameLine(120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColorDisabled", &disabled_color, ImGuiColorEditFlags_Float)) {

		}
		ImGui::Spacing();



		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Checkbox");
	}

	return true;
}

bool ComponentCheckbox::OnHover()
{
	if (active)
	{
		current_color = hover_color;
		tick->GetComponent<ComponentUI>()->current_color = hover_color;
		cross->GetComponent<ComponentUI>()->current_color = hover_color;
	}
	return true;
}

bool ComponentCheckbox::OnClick()
{
	if (active)
	{ 
		current_color = clicked_color;
		tick->GetComponent<ComponentUI>()->current_color = clicked_color;
		cross->GetComponent<ComponentUI>()->current_color = clicked_color;
	}
	return true;
}

bool ComponentCheckbox::OnPressed()
{
	if (active)
	{
		current_color = pressed_color;
		tick->GetComponent<ComponentUI>()->current_color = pressed_color;
		cross->GetComponent<ComponentUI>()->current_color = pressed_color;
	}
	return true;
}

bool ComponentCheckbox::OnRelease()
{
	if (active)
	{ 
		current_color = idle_color;
		tick->GetComponent<ComponentUI>()->current_color = idle_color;
		cross->GetComponent<ComponentUI>()->current_color = idle_color;
	}
	return true;
}

void ComponentCheckbox::SetActive(bool active)
{
	this->active = active;
	if (active) {
		current_color = idle_color;
		tick->GetComponent<ComponentUI>()->current_color = idle_color;
		cross->GetComponent<ComponentUI>()->current_color = idle_color;
	}
	else {
		current_color = disabled_color;
		tick->GetComponent<ComponentUI>()->current_color = disabled_color;
		cross->GetComponent<ComponentUI>()->current_color = disabled_color;
	}
}

ComponentCanvas* ComponentCheckbox::GetCanvas()
{
	ComponentCanvas* canvas = App->objects->GetRoot(true)->GetCanvas();
	if (canvas == nullptr) {
		GameObject* obj = new GameObject(App->objects->GetRoot(false));
		obj->SetName("Canvas");
		obj->AddComponent(new ComponentTransform(obj, { 0,0,0 }, { 0,0,0,0 }, { 1,1,1 }));
		canvas = new ComponentCanvas(obj);
		obj->AddComponent(canvas);
	}
	return canvas;
}
