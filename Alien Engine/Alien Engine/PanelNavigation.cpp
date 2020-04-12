#include "PanelNavigation.h"
#include "ShortCutManager.h"
//#include "ModuleNavigation.h"
#include "Optick/include/optick.h"
#include "ModuleObjects.h"
#include "ModuleNavigation.h"


PanelNavigation::PanelNavigation(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra) :
	Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Panel Navigation", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
	enabled = false;
}

PanelNavigation::~PanelNavigation()
{
}

void PanelNavigation::PanelLogic()
{
	OPTICK_EVENT();

	ImGui::Begin(panel_name.data(), &enabled);
	ImGui::Title("Panel Navigation");
	ImGui::Spacing();
	ImGui::Separator();

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("NavigationTabs", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Areas"))
		{
			ShowAreaTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Bake"))
		{
			ShowBakeTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Object"))
		{
			ShowObjectTab();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	// ----------------------------------------------
	// Used to know when to draw navigation meshes and other recast data on scene panel
	ImDrawList* list = ImGui::GetWindowDrawList();
	if (list != nullptr && list->IdxBuffer.size() > 0) { // TODO: this is shit
		panel_nav_rendered = true;
	}
	else {
		panel_nav_rendered = false;
	}
	// -----------------------------------------------

	ImGui::Separator();

	if(ImGui::CollapsingHeader("Advanced options"))
	{
		const char* drawNames[] = { "DRAWMODE_NAVMESH",
									"DRAWMODE_VOXELS",
									"DRAWMODE_REGION_CONNECTIONS",
									"DRAWMODE_CONTOURS",
									"DRAWMODE_POLYMESH",
									"DRAWMODE_POLYMESH_DETAIL", };
		
		for (uint i = 0; i < (uint)NavDrawMode::MAX_DRAWMODE; ++i)
		{
			ImGui::Checkbox(drawNames[i], &App->nav->drawModes[i]);
		}
	}

	if (enabled != true)
		OnPanelDesactive();

	ImGui::End();
}

void PanelNavigation::ShowObjectTab()
{
	std::list<GameObject*> gos = App->objects->GetSelectedObjects();
	std::vector<GameObject*> mesh_gos;

	for (std::list<GameObject*>::iterator it = gos.begin(); it != gos.end(); ++it) {
		if ((*it)->GetComponent(ComponentType::MESH) != nullptr) { // TODO: needs to check mesh in children?
			mesh_gos.push_back((*it));
		}
	}

	uint num_go_valids = mesh_gos.size();
	ImGui::Text("selected %i gameobject/s", gos.size());
	ImGui::Text("with %i meshes", num_go_valids);

	ImGui::Separator();

	if (num_go_valids == 0)
	{
		ImGui::Text("Select any gameobject with mesh from the scene");
	}
	else
	{
		ImGui::Text("%s", mesh_gos.front()->GetName());
		if (num_go_valids > 1)
		{
			ImGui::SameLine();
			ImGui::Text(num_go_valids > 2 ? "and %i others" : "and %i other", num_go_valids - 1);
		}

		GameObject* first_selected = mesh_gos.front();
		if (ImGui::Checkbox("Navigation Static", &first_selected->nav_data.nav_static))
		{
			// change all valids gameobjects too at once
			for (uint i = 0; i < num_go_valids; ++i)
				mesh_gos[i]->nav_data.nav_static = first_selected->nav_data.nav_static;
		}
	}


	
}

void PanelNavigation::ShowAreaTab()
{
	ImGui::Text("Nothing to see here");
}

void PanelNavigation::ShowBakeTab()
{
	ImGui::Text("Baked Agent Size");
	ImGui::Separator();

	
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	const ImVec2 p = ImGui::GetCursorScreenPos();
	const ImU32 light_blue = ImColor(0.13f, 0.72f, 1.0f, 1.0f);
	const ImU32 mid_blue = ImColor(0.22f, 0.62f, 1.0f, 0.6f);
	const ImU32 dark_blue = ImColor(0.02f, 0.46f, 1.0f, 1.0f);
	const ImU32 white_col = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
	const ImU32 grey_col = ImColor(0.5f, 0.5f, 0.5f, 1.0f);
	
	const float window_w = ImGui::GetWindowContentRegionWidth();
	const float window_center = window_w * 0.5f;
	float rect_max = 100.f; // max box projection size

	float* agent_radius = &App->nav->agentRadius;
	float* agent_height = &App->nav->agentHeight;

	// define the draw canvas and prints a dummy to let space for draw
	const ImVec2 draw_canvas_size(135.f, 125.f);
	ImGui::Dummy(draw_canvas_size);

	// agent calcs draw ---------------------------------
	const float ellipse_relation = 0.3f; //Normally the lower radius is half the bigger one

	//Normalize the proportions
	const float norm = std::sqrtf(*agent_radius * *agent_radius + *agent_height * *agent_height);
	float draw_radius = (*agent_radius / norm);
	float draw_height = (*agent_height / norm);
	double ellip_radius = ((*agent_radius * ellipse_relation) / norm); // to clamp top ellipse border
	
	// search for the difference we have on bottom
	ellip_radius *= rect_max;
	rect_max -= ellip_radius;
	ImGui::Text("rect_max: %f", rect_max);
	//Scale it so it fits the maximum possible space available
	draw_radius *= rect_max;
	draw_height *= rect_max;
	// clamp minimal radius
	draw_radius = fmax(2.0f, draw_radius);
	// ---------------------------------------------------

	// drawing shapes ------------------------------------
	// calc draw positions --------------
	ImVec2 bot_ellipse_c(p.x + window_center, (p.y + draw_canvas_size.y) - (draw_radius * ellipse_relation)); // Always "fixed"
	ImVec2 top_ellipse_center(p.x + window_center, bot_ellipse_c.y - (draw_height)); // mantain the relation from bot to botom center diff
	//ImVec2 top_ellipse_center(p.x + window_center, bot_ellipse_c.y - (draw_height - draw_radius * elipse_relation)); // mantain the relation from bot to botom center diff | faked test
	// body agent rect
	ImVec2 p_min(top_ellipse_center.x - draw_radius, bot_ellipse_c.y);
	ImVec2 p_max(top_ellipse_center.x + draw_radius, top_ellipse_center.y);
	// bottom mid agent line
	float fixed_bottom_agent_line = 175.0f;
	float start_point_x = p.x + window_center - (fixed_bottom_agent_line * 0.5f);
	
	// finally draw the shapes ----------
	// bottom "floor" line
	draw_list->AddLine(ImVec2(p.x, bot_ellipse_c.y), ImVec2(p.x + window_w, bot_ellipse_c.y), white_col);
	// bottom mid agent line (grey line)
	draw_list->AddLine(ImVec2(start_point_x, bot_ellipse_c.y), ImVec2(start_point_x + fixed_bottom_agent_line, bot_ellipse_c.y), grey_col, 2.0f);
	// body agent rect
	draw_list->AddRectFilled(p_min, p_max, mid_blue);
	// bottom ellipse (only thing fixed on point)
	draw_list->AddEllipseFilled(bot_ellipse_c, draw_radius, draw_radius * ellipse_relation, dark_blue, 0.0f, 24);
	// top ellipse
	draw_list->AddEllipseFilled(top_ellipse_center, draw_radius, draw_radius * ellipse_relation, light_blue, 0.0f, 24);

	
	// debug rect to show printable constrained box
	//draw_list->AddRect(ImVec2(p.x + window_center + 40, bot_ellipse_c.y), ImVec2(p.x + window_center - 40, (bot_ellipse_c.y - rect_max)), grey_col); // dyn
	//draw_list->AddRect(ImVec2(p.x + window_center + 40, bot_ellipse_c.y), ImVec2(p.x + window_center - 40, (bot_ellipse_c.y - 100) + (pmin.y - pmax.y)), white_col); // fixed
	// left dif debug indicator box
	/*ImVec2 pmin(p.x + 10, p.y + draw_canvas_size.y);
	ImVec2 pmax(p.x + 20, bot_ellipse_c.y);
	draw_list->AddRect(pmin, pmax, white_col);*/
	
	// menu input variables and widgets ------------------
	ImGui::Separator();

	ImGui::SliderFloat("Agent Radius", agent_radius, 0.01f, 20.0f);
	ImGui::SliderFloat("Agent Height", agent_height, 0.01f, 80.0f);
	if (ImGui::SliderFloat("Max Slope", &App->nav->agentMaxSlope, 0.0f, 60.0f)) {
		if (App->nav->agentMaxSlope > 60.0f)
			App->nav->agentMaxSlope = 60.0f;
	}
	ImGui::SliderFloat("Step Height", &App->nav->agentMaxClimb, 0.0f, 20.0f);

	if (ImGui::Button("Bake", ImVec2(100, 20))) {
		App->nav->Bake();
	}
}

void PanelNavigation::OnPanelDesactive() // TODO: this call doesn't work if the window is NOT docked and is closed by the top right X icon
{
	panel_nav_rendered = false;
}