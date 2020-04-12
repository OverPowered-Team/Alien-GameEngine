#include "PanelNavigation.h"
#include "ShortCutManager.h"
//#include "ModuleNavigation.h"
#include "Optick/include/optick.h"
#include "ModuleObjects.h"
#include "ModuleNavigation.h"

#include <iomanip>


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
	const ImU32 red_col = ImColor(0.9f, 0.2f, 0.2f, 1.0f);
	const float text_height = 12.0f;
	
	const int geo_segments = 24;
	const float window_w = ImGui::GetWindowContentRegionWidth();
	const float window_center = window_w * 0.5f;
	float rect_max = 100.f; // max box projection size

	float* agent_radius = &App->nav->agentRadius;
	float* agent_height = &App->nav->agentHeight;

	// define the draw canvas and prints a dummy to let space for draw
	const ImVec2 draw_canvas_size(135.f, 125.f);
	ImGui::Dummy(draw_canvas_size);

	// agent calcs draw ----------------------------------------------
	const float ellipse_relation = 0.3f; //Normally the lower radius is half the bigger one

	//Normalize the proportions
	const float norm = std::sqrtf(*agent_radius * *agent_radius + *agent_height * *agent_height);
	float draw_radius = (*agent_radius / norm);
	float draw_height = (*agent_height / norm);
	float ellip_radius = ((*agent_radius * ellipse_relation) / norm); // to clamp top ellipse border
	
	// search for the difference we have on bottom
	ellip_radius *= rect_max;
	rect_max -= ellip_radius;
	//ImGui::Text("rect_max: %f", rect_max);
	//Scale it so it fits the maximum possible space available
	draw_radius *= rect_max;
	draw_height *= rect_max;
	// clamp minimal radius
	draw_radius = fmax(2.0f, draw_radius);
	// ----------------------------------------------------------------

	// drawing shapes -------------------------------------------------
	// calc draw positions --------------
	const ImVec2 bot_ellipse_c(p.x + window_center, (p.y + draw_canvas_size.y) - (draw_radius * ellipse_relation)); // Always "fixed"
	const ImVec2 top_ellipse_center(p.x + window_center, bot_ellipse_c.y - (draw_height)); // mantain the relation from bot to botom center diff
	//ImVec2 top_ellipse_center(p.x + window_center, bot_ellipse_c.y - (draw_height - draw_radius * elipse_relation)); // mantain the relation from bot to botom center diff | faked test
	// body agent rect
	const ImVec2 p_min(bot_ellipse_c.x - draw_radius, bot_ellipse_c.y);
	const ImVec2 p_max(top_ellipse_center.x + draw_radius, top_ellipse_center.y);
	// bottom mid agent line
	const float fixed_bottom_agent_line = 175.0f;
	const float start_point_x = p.x + window_center - (fixed_bottom_agent_line * 0.5f);
	
	// finally draw the shapes --------------------
	// bottom "floor" line ------------------------
	draw_list->AddLine(ImVec2(p.x, bot_ellipse_c.y), ImVec2(p.x + window_w - 10.0f, bot_ellipse_c.y), white_col);

	// bottom mid agent line (grey line) ----------
	draw_list->AddLine(ImVec2(start_point_x, bot_ellipse_c.y), ImVec2(start_point_x + fixed_bottom_agent_line, bot_ellipse_c.y), grey_col, 2.0f);

	// body agent rect ----------------------------
	draw_list->AddRectFilled(p_min, p_max, mid_blue);

	// bottom ellipse (only thing fixed on point) -
	draw_list->AddEllipseFilled(bot_ellipse_c, draw_radius, draw_radius * ellipse_relation, dark_blue, 0.0f, geo_segments);

	// step height ---------------------------------------
	float* pstep_h = &App->nav->agentMaxClimb;
	const float step_h = *pstep_h;
	const float height = *agent_height;
	// get real distance from bottom center
	const float step_factor = height / step_h; // qty steps fit on height
	float draw_step_y = (draw_height / step_factor);
	const bool inner_circle = draw_step_y < draw_height; // not draw inner circle if out of height bounds
	draw_step_y = bot_ellipse_c.y - draw_step_y;
	// clamp to max height
	draw_step_y = std::fmax(p.y + 10, draw_step_y);
	
	draw_list->AddLine(ImVec2(p.x, draw_step_y), ImVec2(start_point_x + 1.0f, draw_step_y), inner_circle ? grey_col : red_col , 2.0f); // horizontal line
	draw_list->AddLine(ImVec2(start_point_x, draw_step_y), ImVec2(start_point_x, bot_ellipse_c.y), inner_circle ? grey_col : red_col, 2.0f); // vertical line
	draw_list->AddText(ImVec2(p.x + (start_point_x - p.x) * 0.5f, draw_step_y - text_height), white_col, GetFloatPrecisionString(step_h).c_str());
	if(inner_circle)
		draw_list->AddEllipse(ImVec2(bot_ellipse_c.x, draw_step_y), draw_radius, draw_radius * ellipse_relation, ImColor(1.0f, 1.0f, 1.0f, 0.5f), 0.0f, geo_segments);

	// top ellipse --------------------------------
	draw_list->AddEllipseFilled(top_ellipse_center, draw_radius, draw_radius * ellipse_relation, light_blue, 0.0f, geo_segments);
	// draw radius text and line
	draw_list->AddLine(top_ellipse_center, ImVec2(top_ellipse_center.x + draw_radius - 1.0f, top_ellipse_center.y), white_col);
	draw_list->AddText(ImVec2(top_ellipse_center.x, (top_ellipse_center.y - draw_radius * ellipse_relation) - text_height), 
							  white_col, std::string("R = " + GetFloatPrecisionString(*agent_radius)).c_str());

	// max slope line -----------------------------
	float max_slope_length = 100.0f;
	ImVec2 slope_vx(start_point_x + fixed_bottom_agent_line - 1.0f, bot_ellipse_c.y); // 1 compensate line width
	ImVec2 slope_vy((slope_vx.x + max_slope_length * cosf(DegToRad( App->nav->agentMaxSlope))), 
					 bot_ellipse_c.y + max_slope_length * -sinf(DegToRad(App->nav->agentMaxSlope)));
	
	draw_list->AddLine(slope_vx, slope_vy, grey_col, 2.0f);
	draw_list->AddText(ImVec2(slope_vx.x + text_height, slope_vx.y - text_height), white_col, GetFloatPrecisionString(App->nav->agentMaxSlope).c_str());

	// height text
	draw_list->AddText(ImVec2(bot_ellipse_c.x + draw_radius + 4.0f, (p_max.y - (p_max.y - p_min.y) * 0.5f) - text_height * 0.5f),
		white_col, std::string("H = " + GetFloatPrecisionString(*agent_height)).c_str());
	
	// ----------------------------------------------------------------------------------------
	// debug rect to show printable constrained box
	//draw_list->AddRect(ImVec2(p.x + window_center + 40, bot_ellipse_c.y), ImVec2(p.x + window_center - 40, (bot_ellipse_c.y - rect_max)), grey_col); // dyn
	//draw_list->AddRect(ImVec2(p.x + window_center + 40, bot_ellipse_c.y), ImVec2(p.x + window_center - 40, (bot_ellipse_c.y - 100) + (pmin.y - pmax.y)), white_col); // fixed
	// left dif debug indicator box
	/*ImVec2 pmin(p.x + 10, p.y + draw_canvas_size.y);
	ImVec2 pmax(p.x + 20, bot_ellipse_c.y);
	draw_list->AddRect(pmin, pmax, white_col);*/
	// ----------------------------------------------------------------------------------------

	// menu input variables and widgets ------------------
	ImGui::Separator();
	float drag_speed = 0.02f;
	ImGui::DragFloat("Agent Radius", agent_radius, drag_speed, 0.01f, floatMax, "%.2f");
	ImGui::DragFloat("Agent Height", agent_height, drag_speed, 0.01f, floatMax, "%.2f");
	if (ImGui::SliderFloat("Max Slope", &App->nav->agentMaxSlope, 0.0f, 60.0f)) {
		if (App->nav->agentMaxSlope > 60.0f)
			App->nav->agentMaxSlope = 60.0f;
		else if(App->nav->agentMaxSlope < 0.0f)
			App->nav->agentMaxSlope = 0.0f;
	}
	ImGui::DragFloat("Step Height", pstep_h, drag_speed, 0.01f, floatMax, "%.2f");

	if (ImGui::Button("Bake", ImVec2(100, 20))) {
		App->nav->Bake();
	}
}

const std::string PanelNavigation::GetFloatPrecisionString(float value, const int precision) const
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(precision) << value;
	return ss.str();
}

void PanelNavigation::OnPanelDesactive() // TODO: this call doesn't work if the window is NOT docked and is closed by the top right X icon
{
	panel_nav_rendered = false;
}