#include "Application.h"
#include "PanelCreateMaterial.h"
#include "GameObject.h"
#include "ModuleObjects.h"
#include "Shapes.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ReturnZ.h"
#include "ComponentTransform.h"

PanelCreateMaterial::PanelCreateMaterial(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra) :
	Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("CreateMaterial", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
}

PanelCreateMaterial::~PanelCreateMaterial()
{
}

void PanelCreateMaterial::PanelLogic()
{
	static float x = 0.0f;
	static float y = 0.f;
	static float z = 0.f;

	static int objects_combo = 0;

	ImGui::OpenPopup(panel_name.c_str());
	ImGui::SetNextWindowContentWidth(300);
	if (ImGui::BeginPopupModal(panel_name.c_str(), &enabled, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::IsWindowHovered())
			App->camera->is_scene_hovered = false;
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::SetCursorPosX(10);
		ImGui::InputText("Save Path", path, MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
		//ImGui::Combo("Select Object", &objects_combo, "Cube\0Sphere\0Rock\0Dodecahedron\0Icosahedron\0Octahedron\0Torus\0Klein Bottle\0");

		ImGui::Spacing();

		ImGui::ColorEdit3("Select Color", (float*)&create_color, ImGuiColorEditFlags_Float);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Position");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Columns(3, 0, false);
		ImGui::InputFloat("X", &x, 0, 0, 2);
		ImGui::NextColumn();
		ImGui::InputFloat("Y", &y, 0, 0, 2);
		ImGui::NextColumn();
		ImGui::InputFloat("Z", &z, 0, 0, 2);
		ImGui::NextColumn();
		ImGui::Columns(1);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		SetUniforms();

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Create", { ImGui::GetWindowWidth() - 16,25 }))
		{
			ChangeEnable();
		}

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::EndPopup();
	}
}

void PanelCreateMaterial::SetUniforms()
{
	ImGui::Separator();
	ImGui::Text("Variable name: "); ImGui::SameLine();
	ImGui::InputText("###variable_name", uniform_name, MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
	ImGui::Text("Type: "); ImGui::SameLine();
	if (ImGui::BeginMenu(uniform_type_char))
	{
		if (ImGui::MenuItem("Vec4"))
		{
			uniform_type_char = "vec4";
			
		}
		else if (ImGui::MenuItem("Sampler2D"))
		{
			uniform_type_char = "Sampler2D";
		}
		ImGui::EndMenu();
	}
}