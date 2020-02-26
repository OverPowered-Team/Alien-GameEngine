#include "PanelGame.h"
#include "Color.h"
#include "imgui/imgui.h"
#include "ResourceTexture.h"
#include "imgui/imgui_internal.h"
#include "FileNode.h"
#include "PanelSceneSelector.h"
#include "Viewport.h"
#include "mmgr/mmgr.h"


PanelGame::PanelGame(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
#ifndef GAME_VERSION
	shortcut = App->shortcut_manager->AddShortCut("Panel Game", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
#endif
	game_focused = true;
}

PanelGame::~PanelGame()
{
}

void PanelGame::PanelLogic()
{
	ImGui::Begin(panel_name.data(), &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	/*if (height > ImGui::GetWindowHeight())
	{
		height = ImGui::GetWindowHeight();
		width = (height * 16) / 9;
	}
	else if ((width > ImGui::GetWindowWidth()) || (width > ImGui::GetWindowWidth() && height > ImGui::GetWindowHeight()))
	{
		width = ImGui::GetWindowWidth();
		height = (width * 9) / 16;
	}
	else if ((width < ImGui::GetWindowWidth() && height < ImGui::GetWindowHeight()))
	{
		if ((ImGui::GetWindowHeight() - lastHeight) != 0)
		{
			height = ImGui::GetWindowHeight();
			width = (height * 16) / 9;
		}
		else
		{
			width = ImGui::GetWindowWidth();
			height = (width * 9) / 16;
		}
	}*/

	if (App->objects->game_cameras.empty())
	{
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() * 0.5f) - 80);
		ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f));
		ImGui::Text("No Cameras Available :(");
		App->objects->game_viewport->active = false;
	}
	else {
		viewport_min = ImGui::GetCursorScreenPos();
		viewport_max = ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail();

		// Attach texture to window ----------------------------------------

		current_viewport_size = ImGui::GetContentRegionAvail();
		posX = ImGui::GetWindowPos().x + ImGui::GetCursorPosX();
		posY = ImGui::GetWindowPos().y + ImGui::GetCursorPosY();
		ImGui::Image((ImTextureID)App->objects->game_viewport->GetTexture(), ImVec2(current_viewport_size.x, current_viewport_size.y), ImVec2(0, 1), ImVec2(1, 0));
		width = current_viewport_size.x;
		height = current_viewport_size.y;

		// Set viewport values --------------------------------------------

		App->objects->game_viewport->SetPos(float2(viewport_min.x, viewport_min.y)); // Viewport position after menu bar 

		if (!(current_viewport_size == viewport_size)) // Viewport resize if window size change
		{
			viewport_size = current_viewport_size;
			App->objects->game_viewport->SetSize(viewport_size.x, viewport_size.y);
		}

		App->objects->game_viewport->active = enabled; // Active/disavtive with window active
	}

	/*ImGui::SetCursorPosX((ImGui::GetWindowWidth() - width) * 0.5f);
	ImGui::SetCursorPosY((ImGui::GetWindowHeight() - height) * 0.5f);

	posX = ImGui::GetWindowPos().x + ImGui::GetCursorPosX();
	posY = ImGui::GetWindowPos().y + ImGui::GetCursorPosY() - ImGui::GetCurrentWindow()->TitleBarHeight() - 10;

	if (App->renderer3D->actual_game_camera != nullptr)
	{
		Viewport* viewport = App->objects->game_viewport;
		ImGui::Image((ImTextureID)viewport->GetTexture(), { width,height }, { 0,1 }, { 1,0 });
	}

	if (ImGui::IsWindowHovered()) {
		float2 origin = float2((App->input->GetMousePosition().x - posX), (App->input->GetMousePosition().y - posY));
	}

	lastHeight = ImGui::GetWindowHeight();*/

	if (ImGui::IsWindowFocused()) {
		game_focused = true;
	}
	else {
		game_focused = false;
	}
	
	ImGui::End();
}
