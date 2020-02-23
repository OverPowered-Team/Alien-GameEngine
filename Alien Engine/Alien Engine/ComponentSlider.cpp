#include "ComponentSlider.h"
#include "glew/include/glew.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "Application.h"
#include "PanelGame.h"
#include "ModuleUI.h"
#include "ReturnZ.h"
#include "FileNode.h"
#include "PanelProject.h"
#include "ComponentTransform.h"
#include "ModuleInput.h"
#include "ComponentCamera.h"
#include "ComponentImage.h"


ComponentSlider::ComponentSlider(GameObject* obj) : ComponentUI(obj)
{
	type = ComponentType::UI_SLIDER;

	//---------------------------------------------------------
	dot = new GameObject(game_object_attached);
	dot->SetName("dot");
	dot->AddComponent(new ComponentTransform(dot, { 0,0,0 }, Quat::identity(), { 0.25f,0.25f,0.25f }));
	ComponentImage* comp = new ComponentImage(dot);
	dot->AddComponent(comp);
}

bool ComponentSlider::DrawInspector()
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

	if (ImGui::CollapsingHeader("Slider", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Slider");

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
		RightClickMenu("Slider");
	}

	return true;
}




void ComponentSlider::Reset()
{
}

void ComponentSlider::SetComponent(Component* component)
{
}

void ComponentSlider::Clone(Component* clone)
{
}

void ComponentSlider::SaveComponent(JSONArraypack* to_save)
{
}

void ComponentSlider::LoadComponent(JSONArraypack* to_load)
{
}

bool ComponentSlider::OnHover()
{
	current_color = hover_color;
	return true;
}

bool ComponentSlider::OnClick()
{
	current_color = clicked_color;
	return true;
}

bool ComponentSlider::OnPressed()
{
	ComponentTransform* trans = game_object_attached->GetComponent<ComponentTransform>();
	ComponentTransform* trans_dot = dot->GetComponent<ComponentTransform>();

	float width = (dot->GetComponent<ComponentUI>()->x + ((trans_dot->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F) - (dot->GetComponent<ComponentUI>()->x - ((trans_dot->global_transformation[0][0]/ (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F);
	float width_bg = (x + ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F) - (x - ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F);
	
	int xmotion = App->input->GetMouseXMotion();
	trans_dot->global_transformation[0][3] = trans_dot->global_transformation[0][3] + (xmotion * 0.25f);

	float3 canvas_pos = canvas_trans->GetGlobalPosition();
	float3 object_pos = trans_dot->GetGlobalPosition();
	float3 canvasPivot = { canvas_pos.x - canvas->width * 0.5F, canvas_pos.y + canvas->height * 0.5F, 0 };
	float2 origin = float2((object_pos.x - canvasPivot.x) / (canvas->width), (object_pos.y - canvasPivot.y) / (canvas->height));

	#ifndef GAME_VERSION
		dot->GetComponent<ComponentUI>()->x = origin.x * App->ui->panel_game->width;
		dot->GetComponent<ComponentUI>()->y = -origin.y * App->ui->panel_game->height;
	#else
		x = origin.x * App->window->width;
		y = origin.y * App->window->height;
	#endif

	if ((dot->GetComponent<ComponentUI>()->x + (width * 0.5f)) >= (x + (width_bg * 0.5f)))
	{
		trans_dot->global_transformation[0][3] = trans->global_transformation[0][3] + trans->global_transformation[0][0] - trans_dot->global_transformation[0][0];
	}
	else if (dot->GetComponent<ComponentUI>()->x - (width * 0.5f) <= x - (width_bg * 0.5f))
	{
		trans_dot->global_transformation[0][3] = trans->global_transformation[0][3] - trans->global_transformation[0][0] + trans_dot->global_transformation[0][0];
	}

	current_color = pressed_color;

	return true;
}

bool ComponentSlider::OnRelease()
{
	current_color = idle_color;
	return true;
}

void ComponentSlider::UILogic()
{
	float3 mouse_pos;

#ifndef GAME_VERSION
	mouse_pos = float3((App->input->GetMouseX() - App->ui->panel_game->posX), (App->input->GetMouseY() - App->ui->panel_game->posY), App->input->GetMouseZ());
#else
	mouse_pos = App->input->GetMousePosition();
#endif

	switch (state)
	{
	case Idle:
		if (CheckMouseInside(mouse_pos))
			state = Hover;
		break;
	case Hover:
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			state = Click;
		if (!CheckMouseInside(mouse_pos))
			state = Release;
		break;
	case Click:
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
			state = Pressed;
		break;
	case Pressed:
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && CheckMouseInside(mouse_pos))
			state = Hover;
		else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && !CheckMouseInside(mouse_pos))
			state = Idle;
		break;
	case Release:
		state = Idle;
		break;
	}

	GetValue();
}

bool ComponentSlider::CheckMouseInside(float3 mouse_pos)
{
	ComponentTransform* trans = dot->GetComponent<ComponentTransform>();
#ifdef GAME_VERSION
	return (mouse_pos.x >= dot->GetComponent<ComponentUI>()->x - ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->window->width) * 0.5F && mouse_pos.x <= dot->GetComponent<ComponentUI>()->x + ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->window->width) * 0.5F && mouse_pos.y >= dot->GetComponent<ComponentUI>()->y - ((trans->global_transformation[1][1] / (canvas->height * 0.5F) * App->window->height) * 0.5F) && mouse_pos.y <= y + ((trans->global_transformation[1][1] / (canvas->height * 0.5F)) * App->window->height) * 0.5F);
#else
	return (mouse_pos.x >= dot->GetComponent<ComponentUI>()->x - ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F && mouse_pos.x <= dot->GetComponent<ComponentUI>()->x + ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F && mouse_pos.y >= dot->GetComponent<ComponentUI>()->y - ((trans->global_transformation[1][1] / (canvas->height * 0.5F) * App->ui->panel_game->height) * 0.5F) && mouse_pos.y <= dot->GetComponent<ComponentUI>()->y + ((trans->global_transformation[1][1] / (canvas->height * 0.5F)) * App->ui->panel_game->height) * 0.5F);
#endif
	//return false;
}

float ComponentSlider::GetValue()
{
	ComponentTransform* trans = game_object_attached->GetComponent<ComponentTransform>();
	ComponentTransform* trans_dot = dot->GetComponent<ComponentTransform>();


	/*float ipos_bar = trans_dot->global_transformation[0][3] - (trans_dot->global_transformation[0][0] * 0.5f);
	float fixed_pos = trans->global_transformation[0][3] - trans->global_transformation[0][0];
	float fpos_bar = trans->global_transformation[0][3] + trans->global_transformation[0][0] - trans_dot->global_transformation[0][0];
	float final_pos = (ipos_bar - fixed_pos) / (fpos_bar - fixed_pos);*/




	float startPos = (trans->global_transformation[0][3] - (trans->global_transformation[0][0] * 0.5f));
	float endPos = (trans->global_transformation[0][3] + trans->global_transformation[0][0] - (trans_dot->global_transformation[0][0] * 0.5F));
	float thumbPos = trans_dot->global_transformation[0][3] + trans_dot->global_transformation[0][0] * 0.5;
	LOG_ENGINE("SLIDER VALUE: %f", (thumbPos - startPos) / (endPos - startPos));
	return((thumbPos - startPos) / (endPos - startPos));


	

	//return final_pos;
}
