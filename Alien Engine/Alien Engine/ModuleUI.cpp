#include "Globals.h"
#include "Application.h"
#include "ModuleUI.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include <gl/GL.h>
#include "ShortCutManager.h"
#include "PanelAbout.h"
#include "Viewport.h"
#include "PanelConfig.h"
#include "PanelBuild.h"
#include "PanelConsole.h"
#include "PanelCreateObject.h"
#include "PanelHierarchy.h"
#include "PanelRender.h"
#include "Time.h"
#include "SDL/include/SDL_assert.h"
#include "ModuleObjects.h"
#include "PanelLayout.h"
#include "PanelInspector.h"
#include "PanelProject.h"
#include "PanelSceneSelector.h"
#include "PanelScene.h"
#include "PanelGame.h"
#include "PanelTextEditor.h"
#include "PanelAnimTimeline.h"
#include "PanelAnimator.h"
#include "PanelPhysics.h"
#include <string>
#include "ResourceTexture.h"
#include "ReturnZ.h"
#include "Event.h"
#include "PanelTextEditor.h"
#include "PanelParticleSystem.h"
#include "ModuleObjects.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "ModuleCamera3D.h"
#include "ComponentUI.h"
#include <fstream>
#include "mmgr/mmgr.h"
#include "Optick/include/optick.h"

ModuleUI::ModuleUI(bool start_enabled) : Module(start_enabled)
{
	name.assign("UI");
}

ModuleUI::~ModuleUI()
{

}

// Load assets
bool ModuleUI::Start()
{
	OPTICK_EVENT();
	LOG_ENGINE("Loading UI assets");
	bool ret = true;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.IniFilename = NULL;
	io.WantSaveIniSettings = false;

	ImGuiStyle& style = ImGui::GetStyle();

	style.MaxColumnSeparation= 10;
	style.TitleSeparation = 20;
	style.SubTitleSeparation = 3;
	style.SeparationType = ImGuiSeparationType::ImGui_WindowSeparation;


	ChangeStyle(App->window->style);
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);

	InitPanels();
	InitShortCuts();

	LoadLayouts();
	LoadActiveLayout();

	return ret;
}


// Load assets
bool ModuleUI::CleanUp()
{
	OPTICK_EVENT();
	LOG_ENGINE("Unloading UI scene");

	if (!need_to_save_layouts) // just save to know which is active
		SaveLayoutsActive();
	else
		SaveAllLayouts();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	std::vector<Panel*>::iterator item = panels.begin();
	for (; item != panels.end(); ++item) {
		if ((*item) != nullptr) {
			delete *item;
			*item = nullptr;
		}
	}
	panels.clear();

	std::vector<Layout*>::iterator it = layouts.begin();
	for (; it != layouts.end(); ++it) {
		if ((*it) != nullptr) {
			delete* it;
			*it = nullptr;
		}
	}
	layouts.clear();

	return true;
}

void ModuleUI::LoadConfig(JSONfilepack*& config)
{
	OPTICK_EVENT();

	for (uint i = 0; i < 3; ++i) {
		panel_config_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelConfig", i);
		panel_build_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelBuild", i);
		panel_about_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelAbout", i);
		panel_create_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelCreate", i);
		panel_project_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelCreate", i);
		panel_hierarchy_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelProject", i);
		panel_console_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelConsole", i);
		panel_inspector_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelInspector", i);
		panel_layout_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelLayout", i);
		panel_render_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelRender", i);
		panel_game_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.Game", i);
		panel_scene_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelScene", i);
		panel_scene_selector_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelSceneSelector", i);
		panel_text_edit_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelTextEditor", i);
		panel_animtimeline_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelAnimTimeline", i);
		panel_animator_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.PanelAnimator", i);
		shortcut_demo_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.ImGuiDemo", i);
		shortcut_report_bug_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.ReportBug", i);
		shortcut_view_mesh_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.ViewMesh", i);
		shortcut_wireframe_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.WireframeMode", i);
		shortcut_view_grid_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.ShowGrid", i);
		shortcut_view_normal_vertex_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.ShowNormalVertex", i);
		shortcut_view_normal_face_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.ShowNormalFace", i);
		shortcut_save_scene_as_new_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.SaveSceneAsNew", i);
		shortcut_load_scene_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.LoadScene", i);
		shortcut_new_scene_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.NewScene", i);
		shortcut_outline_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.EnableOutline", i);
		shortcut_AABB_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.AABB", i);
		shortcut_OBB_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.OBB", i);
		shortcut_octree_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.ViewOctree", i);
		shortcut_cntrlZ_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.GoBack", i);
		shortcut_cntrlY_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.GoFordward", i);
		shortcut_duplicate_object_codes[i] = (SDL_Scancode)(uint)config->GetArrayNumber("Configuration.UI.ShortCuts.DuplicateObject", i);
	}
	if (panel_about != nullptr) {
		panel_about->shortcut->SetShortcutKeys(panel_about_codes[0], panel_about_codes[1], panel_about_codes[2]);
		panel_build->shortcut->SetShortcutKeys(panel_build_codes[0], panel_build_codes[1], panel_build_codes[2]);
		panel_config->shortcut->SetShortcutKeys(panel_config_codes[0], panel_config_codes[1], panel_config_codes[2]);
		panel_scene_selector->shortcut->SetShortcutKeys(panel_scene_selector_codes[0], panel_scene_selector_codes[1], panel_scene_selector_codes[2]);
		panel_project->shortcut->SetShortcutKeys(panel_project_codes[0], panel_project_codes[1], panel_project_codes[2]);
		panel_console->shortcut->SetShortcutKeys(panel_console_codes[0], panel_console_codes[1], panel_console_codes[2]);
		panel_render->shortcut->SetShortcutKeys(panel_render_codes[0], panel_render_codes[1], panel_render_codes[2]);
		panel_hierarchy->shortcut->SetShortcutKeys(panel_hierarchy_codes[0], panel_hierarchy_codes[1], panel_hierarchy_codes[2]);
		panel_create_object->shortcut->SetShortcutKeys(panel_create_codes[0], panel_create_codes[1], panel_create_codes[2]);
		panel_inspector->shortcut->SetShortcutKeys(panel_inspector_codes[0], panel_inspector_codes[1], panel_inspector_codes[2]);
		panel_scene->shortcut->SetShortcutKeys(panel_scene_codes[0], panel_scene_codes[1], panel_scene_codes[2]);
		panel_text_editor->shortcut->SetShortcutKeys(panel_text_edit_codes[0], panel_text_edit_codes[1], panel_text_edit_codes[2]);
		panel_game->shortcut->SetShortcutKeys(panel_game_codes[0], panel_game_codes[1], panel_game_codes[2]);
		panel_layout->shortcut->SetShortcutKeys(panel_layout_codes[0], panel_layout_codes[1], panel_layout_codes[2]);
		panel_animtimeline->shortcut->SetShortcutKeys(panel_animtimeline_codes[0], panel_animtimeline_codes[1], panel_animtimeline_codes[2]);
		panel_animator->shortcut->SetShortcutKeys(panel_animator_codes[0], panel_animator_codes[1], panel_animator_codes[2]);
		shortcut_demo->SetShortcutKeys(shortcut_demo_codes[0], shortcut_demo_codes[1], shortcut_demo_codes[2]);
		shortcut_report_bug->SetShortcutKeys(shortcut_report_bug_codes[0], shortcut_report_bug_codes[1], shortcut_report_bug_codes[2]);
		shortcut_view_mesh->SetShortcutKeys(shortcut_view_mesh_codes[0], shortcut_view_mesh_codes[1], shortcut_view_mesh_codes[2]);
		shortcut_wireframe->SetShortcutKeys(shortcut_wireframe_codes[0], shortcut_wireframe_codes[1], shortcut_wireframe_codes[2]);
		shortcut_view_grid->SetShortcutKeys(shortcut_view_grid_codes[0], shortcut_view_grid_codes[1], shortcut_view_grid_codes[2]);
		shortcut_view_normal_vertex->SetShortcutKeys(shortcut_view_normal_vertex_codes[0], shortcut_view_normal_vertex_codes[1], shortcut_view_normal_vertex_codes[2]);
		shortcut_view_normal_face->SetShortcutKeys(shortcut_view_normal_face_codes[0], shortcut_view_normal_face_codes[1], shortcut_view_normal_face_codes[2]);
		shortcut_load_scene->SetShortcutKeys(shortcut_load_scene_codes[0], shortcut_load_scene_codes[1], shortcut_load_scene_codes[2]);
		shortcut_save_scene_as_new->SetShortcutKeys(shortcut_save_scene_as_new_codes[0], shortcut_save_scene_as_new_codes[1], shortcut_save_scene_as_new_codes[2]);
		shortcut_new_scene->SetShortcutKeys(shortcut_new_scene_codes[0], shortcut_new_scene_codes[1], shortcut_new_scene_codes[2]);
		shortcut_outline->SetShortcutKeys(shortcut_outline_codes[0], shortcut_outline_codes[1], shortcut_outline_codes[2]);
		shortcut_AABB->SetShortcutKeys(shortcut_AABB_codes[0], shortcut_AABB_codes[1], shortcut_AABB_codes[2]);
		shortcut_OBB->SetShortcutKeys(shortcut_OBB_codes[0], shortcut_OBB_codes[1], shortcut_OBB_codes[2]);
		shortcut_octree->SetShortcutKeys(shortcut_octree_codes[0], shortcut_octree_codes[1], shortcut_octree_codes[2]);
		shortcut_cntrlZ->SetShortcutKeys(shortcut_cntrlZ_codes[0], shortcut_cntrlZ_codes[1], shortcut_cntrlZ_codes[2]);
		shortcut_cntrlY->SetShortcutKeys(shortcut_cntrlY_codes[0], shortcut_cntrlY_codes[1], shortcut_cntrlY_codes[2]);
		shortcut_duplicate_object->SetShortcutKeys(shortcut_duplicate_object_codes[0], shortcut_duplicate_object_codes[1], shortcut_duplicate_object_codes[2]);

		// OrderShortCuts must be called after all shortcuts have been created!! Victor read this...
		App->shortcut_manager->OrderShortCuts();
	}
}

void ModuleUI::SaveConfig(JSONfilepack*& config)
{
	OPTICK_EVENT();

	for (uint i = 0; i < 3; ++i) {
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelConfig", (uint)panel_config->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelConfig", (uint)panel_build->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelProject", (uint)panel_project->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelAbout", (uint)panel_about->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelHierarchy", (uint)panel_hierarchy->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelRender", (uint)panel_render->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelTextEditor", (uint)panel_text_editor->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelInspector", (uint)panel_inspector->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelConsole", (uint)panel_console->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelCreate", (uint)panel_create_object->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelScene", (uint)panel_scene->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelLayout", (uint)panel_layout->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelAnimTimeline", (uint)panel_animtimeline->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelAnimator", (uint)panel_animator->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.WireframeMode", (uint)shortcut_wireframe->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.ViewMesh", (uint)shortcut_view_mesh->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.ReportBug", (uint)shortcut_report_bug->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.ImGuiDemo", (uint)shortcut_demo->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.ShowGrid", (uint)shortcut_view_grid->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.ShowNormalVertex", (uint)shortcut_view_normal_vertex->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.ShowNormalFace", (uint)shortcut_view_normal_face->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.LoadScene", (uint)shortcut_load_scene->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.PanelSceneSelector", (uint)panel_scene_selector->shortcut->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.SaveSceneAsNew", (uint)shortcut_save_scene_as_new->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.NewScene", (uint)shortcut_new_scene->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.EnableOutline", (uint)shortcut_outline->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.AABB", (uint)shortcut_AABB->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.OBB", (uint)shortcut_OBB->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.ViewOctree", (uint)shortcut_octree->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.GoBack", (uint)shortcut_cntrlZ->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.GoFordward", (uint)shortcut_cntrlY->GetScancode(i));
		config->SetArrayNumber("Configuration.UI.ShortCuts.DuplicateObject", (uint)shortcut_duplicate_object->GetScancode(i));
	}
}
void ModuleUI::LoadLayouts()
{
	OPTICK_EVENT();

	JSONfilepack* json_layout = App->GetJSONLayout();
	JSONArraypack* arr_layouts = json_layout->GetArray("Layouts");

	for (uint i = 0; i < arr_layouts->GetArraySize(); ++i) {
		Layout* layout = new Layout();
		layout->name = arr_layouts->GetString("Name");
		layout->path = arr_layouts->GetString("Path");
		layout->active = arr_layouts->GetBoolean("Active");

		if (layout->active && active_layout == nullptr)
			active_layout = layout;
		else if (layout->active)
			layout->active = false;

		std::vector<Panel*>::iterator panel = panels.begin();
		for (; panel != panels.end(); ++panel) {
			if (*panel != nullptr) {
				layout->panels_enabled.push_back(arr_layouts->GetBoolean((*panel)->GetPanelName().data()));
			}
		}

		if (App->file_system->Exists(layout->path.data())) {
			layouts.push_back(layout);
		}
		else {
			delete layout;
			layout = nullptr;
			need_to_save_layouts = true;
		}
		arr_layouts->GetAnotherNode();
	}
}

void ModuleUI::SaveAllLayouts()
{
	OPTICK_EVENT();

	JSONfilepack* json_layout = App->GetJSONLayout();
	App->DeleteJSONfile(json_layout);
	App->file_system->Remove("Configuration/LayoutsInfo.json");
	json_layout = App->CreateJSONFile("Configuration/LayoutsInfo.json");

	json_layout->StartSave();

	JSONArraypack* arr_layouts = json_layout->InitNewArray("Layouts");
	std::vector<Layout*>::iterator item = layouts.begin();
	for (; item != layouts.end(); ++item) {
		if (*item != nullptr) {
			arr_layouts->SetAnotherNode();
			arr_layouts->SetBoolean("Active", (*item)->active);
			arr_layouts->SetString("Name", (*item)->name.data());
			arr_layouts->SetString("Path", (*item)->path.data());

			std::vector<Panel*>::iterator panel = panels.begin();
			for (; panel != panels.end(); ++panel) {
				if (*panel != nullptr) {
					bool enabled = (*panel)->IsEnabled();
					arr_layouts->SetBoolean((*panel)->GetPanelName().data(), enabled);
				}
			}
		}
	}
	json_layout->FinishSave();
}

void ModuleUI::SaveLayout(Layout* layout, bool is_new)
{
	OPTICK_EVENT();

	JSONfilepack* json_layout = App->GetJSONLayout();
	json_layout->StartSave();

	JSONArraypack* arr_layout = json_layout->GetArray("Layouts");

	if (is_new) {
		arr_layout->SetAnotherNode();
	}
	else {
		for (uint i = 0; i < arr_layout->GetArraySize(); ++i) {
			if (App->StringCmp(arr_layout->GetString("Name"), layout->name.data())) {
				break;
			}
			arr_layout->GetAnotherNode();
		}
	}

	arr_layout->SetBoolean("Active", layout->active);
	arr_layout->SetString("Name", layout->name.data());
	arr_layout->SetString("Path", layout->path.data());
	layout->panels_enabled.clear();
	std::vector<Panel*>::iterator panel = panels.begin();
	for (; panel != panels.end(); ++panel) {
		if (*panel != nullptr) {
			bool enabled = (*panel)->IsEnabled();
			arr_layout->SetBoolean((*panel)->GetPanelName().data(), enabled);
			layout->panels_enabled.push_back(enabled);
		}
	}

	json_layout->FinishSave();
	if (!is_new) {
		ImGui::SaveIniSettingsToDisk(layout->path.data());
	}
}

void ModuleUI::SaveLayoutsActive()
{
	OPTICK_EVENT();

	// just save the active attribut & name to know in the next session which layout was active

	JSONfilepack* json_layout = App->GetJSONLayout();
	json_layout->StartSave();

	JSONArraypack* arr_layout = json_layout->GetArray("Layouts");

	std::vector<Layout*>::iterator item = layouts.begin();
	for (; item != layouts.end(); ++item) {
		if (*item != nullptr) {
			arr_layout->SetBoolean("Active", (*item)->active);
			arr_layout->SetString("Name", (*item)->name.data());
			arr_layout->GetAnotherNode();
		}
	}
	json_layout->FinishSave();
}

void ModuleUI::CreateScriptFile(const int& type, bool to_export, const char* name)
{
	OPTICK_EVENT();
	std::string file_output = std::string(HEADER_SCRIPTS_FILE + std::string(name) + std::string(".h"));
	switch (type) {
	case 1: { // class
		if (to_export) {
			App->file_system->Copy(EXPORT_FILE_CLASS_TEMPLATE, file_output.data());
		}
		else {
			App->file_system->Copy(CLASS_FILE_TEMPLATE, file_output.data());
		}
		break; }
	case 2: { // struct
		if (to_export) {
			App->file_system->Copy(EXPORT_FILE_STRUCT_TEMPLATE, file_output.data());
		}
		else {
			App->file_system->Copy(STRUCT_FILE_TEMPLATE, file_output.data());
		}
		break; }
	default:
		break;
	}
	
	// change de .h
	std::ifstream file(file_output);
	std::string file_str;
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			while (line.find("DataAlienTypeName") != std::string::npos) {
				line.replace(line.find("DataAlienTypeName"), 17, std::string(name));
			}
			if (file_str.empty()) {
				file_str = line;
			}
			else {
				file_str += std::string("\n") + line;
			}
		}
		file.close();
	}
	App->file_system->Save(file_output.data(), file_str.data(), file_str.size());

	// change the cpp
	std::string cpp_path; 
	if (type == 1 && to_export) {
		cpp_path = std::string(HEADER_SCRIPTS_FILE + std::string(name) + std::string(".cpp"));
		App->file_system->Copy(CPP_ALIEN_FILE_TEMPLATE, cpp_path.data());
	}
	else {
		cpp_path = std::string(HEADER_SCRIPTS_FILE + std::string(name) + std::string(".cpp"));
		App->file_system->Copy(CPP_FILE_TEMPLATE, cpp_path.data());
	}

	std::ifstream cpp(cpp_path);
	std::string cpp_str;
	if (cpp.is_open()) {
		std::string line;
		while (std::getline(cpp, line)) {
			while (line.find("DataAlienTypeName") != std::string::npos) {
				line.replace(line.find("DataAlienTypeName"), 17, std::string(name));
			}
			if (cpp_str.empty()) {
				cpp_str = line;
			}
			else {
				cpp_str += std::string("\n") + line;
			}
		}
		cpp.close();
	}
	App->file_system->Save(cpp_path.data(), cpp_str.data(), cpp_str.size());

}

update_status ModuleUI::PreUpdate(float dt)
{
	OPTICK_EVENT();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (Time::state == Time::GameState::PLAY_ONCE) {
		Time::Pause();
}
	if (change_game_state.first) {
		switch (change_game_state.second) {
		case Time::GameState::NONE: {
			Time::Stop();
			break; }
		case Time::GameState::PLAY: {
			Time::Play();
			break; }
		case Time::GameState::PLAY_ONCE: {
			Time::PlayOnce();
			break; }
		case Time::GameState::PAUSE: {
			Time::Pause();
			break; }
		}
		change_game_state.first = false;
	}

	return UPDATE_CONTINUE;
}


void ModuleUI::Draw() 
{
	OPTICK_EVENT();
	if (show_demo_wndow) {
		ImGui::ShowDemoWindow(&show_demo_wndow);
	}
	MainMenuBar();
	BackgroundDockspace();
	SecondMenuBar();
	UpdatePanels();
	if (creating_script) {
		CreateNewScriptPopUp();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	//Viewports
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

void ModuleUI::MainMenuBar()
{
	OPTICK_EVENT();
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Create New Scene", shortcut_new_scene->GetNameScancodes()))
		{
			panel_scene_selector->OrganizeSave(PanelSceneSelector::SceneSelectorState::CREATE_NEW_SCENE);
		}
		if (ImGui::MenuItem("Save Scene", panel_scene_selector->shortcut->GetNameScancodes()))
		{
			panel_scene_selector->OrganizeSave(PanelSceneSelector::SceneSelectorState::SAVE_SCENE);
		}	
		if (ImGui::MenuItem("Save Scene As", shortcut_save_scene_as_new->GetNameScancodes(), nullptr, App->objects->GetGlobalRoot()->children.size() < 2))
		{
			panel_scene_selector->OrganizeSave(PanelSceneSelector::SceneSelectorState::SAVE_AS_NEW);
		}
		if (ImGui::MenuItem("Load Scene", shortcut_load_scene->GetNameScancodes(), nullptr, !App->objects->prefab_scene))
		{
			panel_scene_selector->OrganizeSave(PanelSceneSelector::SceneSelectorState::LOAD_SCENE);
		}
		if (ImGui::MenuItem("Import"))
		{

		}
		if (ImGui::MenuItem("Build Settings", panel_build->shortcut->GetNameScancodes()))
		{
			panel_build->ChangeEnable();
		}
		if (ImGui::MenuItem("Close", "Alt + F4"))
		{
			App->QuitApp();
		}

		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View"))
	{
		for (Panel* panel : panels)
		{
			if (panel->shortcut != nullptr)
			{
				if (ImGui::MenuItem(panel->GetName().c_str(), panel->shortcut->GetNameScancodes()))
				{
					panel->ChangeEnable();
				}
			}
			else
			{
				if (ImGui::MenuItem(panel->GetName().c_str()))
				{
					panel->ChangeEnable();
				}
			}
		}

		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Assets"))
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Animator Controller"))
			{
				App->resources->CreateAsset(FileDropType::ANIM_CONTROLLER);
			}
			if (ImGui::MenuItem("Script")) {
				App->ui->creating_script = true;
			}
			if (ImGui::MenuItem("Material")) {
				App->resources->CreateMaterial("New Material");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Create"))
	{
		if (ImGui::MenuItem("Create Empty GameObject"))
		{
			App->objects->CreateEmptyGameObject(nullptr);
		}
		if (ImGui::MenuItem("Cube"))
		{
			App->objects->CreateBasePrimitive(PrimitiveType::CUBE);
		}
		if (ImGui::MenuItem("Sphere"))
		{
			App->objects->CreateBasePrimitive(PrimitiveType::SPHERE_ALIEN);
		}
		if (ImGui::MenuItem("Rock"))
		{
			App->objects->CreateBasePrimitive(PrimitiveType::ROCK);
		}
		if (ImGui::MenuItem("Torus"))
		{
			App->objects->CreateBasePrimitive(PrimitiveType::TORUS);
		}
		if (ImGui::BeginMenu("Other"))
		{
			if (ImGui::MenuItem("Dodecahedron"))
			{
				App->objects->CreateBasePrimitive(PrimitiveType::DODECAHEDRON);
			}
			if (ImGui::MenuItem("Octahedron"))
			{
				App->objects->CreateBasePrimitive(PrimitiveType::OCTAHEDRON);
			}
			if (ImGui::MenuItem("Icosahedron"))
			{
				App->objects->CreateBasePrimitive(PrimitiveType::ICOSAHEDRON);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Point light"))
			{
				App->objects->CreateLight(LightTypeObj::POINT);
			}
			if (ImGui::MenuItem("Spot light"))
			{
				App->objects->CreateLight(LightTypeObj::SPOT);
			}
			if (ImGui::MenuItem("Directional light"))
			{
				App->objects->CreateLight(LightTypeObj::DIRECTIONAL);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("UI"))
		{
			if (ImGui::MenuItem("Image"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_IMAGE);
			}
			if (ImGui::MenuItem("Button"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_BUTTON);
			}
			if (ImGui::MenuItem("Checkbox"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_CHECKBOX);
			}
			if (ImGui::MenuItem("Text"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_TEXT);
			}
			if (ImGui::MenuItem("Slider"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_SLIDER);
			}
			if (ImGui::MenuItem("Bar"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_BAR);
			}
			if (ImGui::MenuItem("Animated Image"))
			{
				App->objects->CreateBaseUI(ComponentType::UI_ANIMATED_IMAGE);
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Create...", panel_create_object->shortcut->GetNameScancodes()))
		{
			panel_create_object->ChangeEnable();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Layout")) {
		if (ImGui::MenuItem("Save Actual Layout", active_layout->name.data())) {
			SaveLayout(active_layout, false);
		}
		if (ImGui::MenuItem("Edit Layouts", panel_layout->shortcut->GetNameScancodes())) {
			panel_layout->ChangeEnable();
			static_cast<PanelLayout*>(panel_layout)->is_editor_panel = true;
		}
		if (ImGui::MenuItem("Save Actual Distribution", "Create New Layout")) {
			panel_layout->ChangeEnable();
			static_cast<PanelLayout*>(panel_layout)->is_editor_panel = false;
		}
		if (ImGui::BeginMenu("Set Layout"))
		{
			std::vector<Layout*>::iterator item = layouts.begin();
			for (; item != layouts.end(); ++item) {
				if (*item != nullptr) {
					if (ImGui::MenuItem((*item)->name.data())) {
						ResetImGui();
						active_layout->active = false;
						active_layout = *item;
						(*item)->active = true;
						LoadActiveLayout();
						ImGui::NewFrame();
						return;
					}
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("About", panel_about->shortcut->GetNameScancodes()))
		{
			panel_about->ChangeEnable();
		}
		if (ImGui::MenuItem("Show Gui Demo", shortcut_demo->GetNameScancodes()))
		{
			ChangeEnableDemo();
		}
		if (ImGui::MenuItem("Documentation"))
		{
			App->OpenWebsite("https://contrasnya.wixsite.com/aliengameengine");
		}
		if (ImGui::MenuItem("Report a bug", shortcut_report_bug->GetNameScancodes()))
		{
			ReportBug();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void ModuleUI::SecondMenuBar()
{
	OPTICK_EVENT();
	ImGui::SetNextWindowSize(ImVec2(App->window->width,52));
	ImGui::Begin("## Tools", (bool*)false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse);

	ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f) - 18);
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	if (ImGui::ImageButton((ImTextureID)App->resources->icons.undo->id, ImVec2(30, 30)))
	{
		ReturnZ::GoBackOneAction();
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text(std::string("Undo " + std::string(shortcut_cntrlZ->GetNameScancodes())).data());
		ImGui::EndTooltip();
	}
	ImGui::SameLine();

	if (ImGui::ImageButton((ImTextureID)App->resources->icons.redo->id, ImVec2(30, 30)))
	{
		ReturnZ::GoFordwardOneAction();
	}
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text(std::string("Redo " + std::string(shortcut_cntrlY->GetNameScancodes())).data());
		ImGui::EndTooltip();
	}
	// Vertical Separator

	ImGui::SameLine();
	ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f) - 5);
	ImGui::Text("|");
	ImGui::SameLine();

	// Transform Buttons
	if (panel_scene->guizmo_operation == ImGuizmo::OPERATION::TRANSLATE)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.2F, 0.6F, 1, 1 });
	}
	else
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.15F, 0.15F, 0.15F, 1 });

	ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f) - 17);
	if (ImGui::ImageButton((ImTextureID)App->resources->icons.move_transform->id, ImVec2(30, 30)))
	{
		panel_scene->guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
	}
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Move (W)");
		ImGui::EndTooltip();
	}
	ImGui::SameLine();

	if (panel_scene->guizmo_operation == ImGuizmo::OPERATION::ROTATE)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.2F, 0.6F, 1, 1 });
	}
	else
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.15F, 0.15F, 0.15F, 1 });

	if (ImGui::ImageButton((ImTextureID)App->resources->icons.rotate_transform->id, ImVec2(30, 30)))
	{
		panel_scene->guizmo_operation = ImGuizmo::OPERATION::ROTATE;
	}
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Rotate (E)");
		ImGui::EndTooltip();
	}
	ImGui::SameLine();

	if (panel_scene->guizmo_operation == ImGuizmo::OPERATION::SCALE)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.2F, 0.6F, 1, 1 });
	}
	else
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.15F, 0.15F, 0.15F, 1 });

	if (ImGui::ImageButton((ImTextureID)App->resources->icons.scale_transform->id, ImVec2(30, 30)))
	{
		panel_scene->guizmo_operation = ImGuizmo::OPERATION::SCALE;
	}
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Scale (R)");
		ImGui::EndTooltip();
	}

	// Vertical Separator

	ImGui::SameLine();
	ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f) - 5);
	ImGui::Text("|");
	ImGui::SameLine();

	// Transform Modes
	ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f) - 17);
	if (panel_scene->guizmo_mode == ImGuizmo::MODE::WORLD)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.2F, 0.6F, 1, 1 });
	}
	else
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.15F, 0.15F, 0.15F, 1 });

	if (ImGui::ImageButton((ImTextureID)App->resources->icons.global->id, ImVec2(30, 30)))
	{
		panel_scene->guizmo_mode = ImGuizmo::MODE::WORLD;
	}
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("World Mode (Shift+W)");
		ImGui::EndTooltip();
	}
	ImGui::SameLine();

	if (panel_scene->guizmo_mode == ImGuizmo::MODE::LOCAL)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.2F, 0.6F, 1, 1 });
	}
	else
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.15F, 0.15F, 0.15F, 1 });

	if (ImGui::ImageButton((ImTextureID)App->resources->icons.local->id, ImVec2(30, 30)))
	{
		panel_scene->guizmo_mode = ImGuizmo::MODE::LOCAL;
	}
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Local Mode (Shift+L)");
		ImGui::EndTooltip();
	}


	// Vertical Separator

	ImGui::SameLine();
	ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f) - 5);
	ImGui::Text("|");
	float separator = ((float)App->window->width * (1.f / 48.f));
	ImGui::SameLine(0, separator);

	// Camera Combo
	ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f) - 7);

	static int camera_combo = 0;
	std::string combo_cameras_name;

	if (App->objects->game_cameras.empty())
	{
		actual_name = "No selected camera";
	}
	else
	{
		actual_name = App->renderer3D->actual_game_camera->game_object_attached->GetName();
	}

	ImGui::PushItemWidth(175);
	if (ImGui::BeginCombo("Current Game Camera", actual_name))
	{
		for (std::vector<ComponentCamera*>::iterator iter = App->objects->game_cameras.begin(); iter != App->objects->game_cameras.end(); ++iter)
		{
			bool is_selected = (actual_name == (*iter)->game_object_attached->GetName());
			if (ImGui::Selectable((*iter)->game_object_attached->GetName(), is_selected))
			{
				actual_name = (*iter)->game_object_attached->GetName();
			}
			if (ImGui::IsItemClicked())
			{
				ImGui::SetItemDefaultFocus();
				App->renderer3D->actual_game_camera = (*iter);
				App->objects->game_viewport->SetCamera(*iter);
			}
		}
		ImGui::EndCombo();
	}


	// Vertical Separator

	separator = ((float)App->window->width * (1.f / 32.f));
	ImGui::SameLine(0, separator);
	ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f) - 7);
	ImGui::Text("|");
	ImGui::SameLine();

	// Play Buttons
	ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f) - 17);
	if (Time::state == Time::GameState::PLAY)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.2F, 0.6F, 1, 1 });
	}
	else
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.15F, 0.15F, 0.15F, 1 });

	if (ImGui::ImageButton((ImTextureID)App->resources->icons.play->id, ImVec2(30, 30)))
	{
		change_game_state.first = true;
		change_game_state.second = Time::GameState::PLAY;
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();

	if (Time::state == Time::GameState::PAUSE)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.2F, 0.6F, 1, 1 });
	}
	else
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.15F, 0.15F, 0.15F, 1 });

	if (ImGui::ImageButton((ImTextureID)App->resources->icons.pause->id, ImVec2(30, 30)) && Time::IsInGameState())
	{
		change_game_state.first = true;
		change_game_state.second = Time::GameState::PAUSE;
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();

	if (Time::state == Time::GameState::PLAY_ONCE)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.2F, 0.6F, 1, 1 });
	}
	else
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.15F, 0.15F, 0.15F, 1 });

	if (ImGui::ImageButton((ImTextureID)App->resources->icons.once->id, ImVec2(30, 30)) && Time::IsInGameState())
	{
		change_game_state.first = true;
		change_game_state.second = Time::GameState::PLAY_ONCE;
	}
	ImGui::PopStyleColor();

	// Vertical Separator

	ImGui::SameLine();
	ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f) - 7);
	ImGui::Text("|");
	ImGui::SameLine();

	//ImGui::SameLine();

	//ImGui::Text("Time Since Start: %f", Time::time_since_start);
	//ImGui::SameLine();
	//ImGui::Text("Game Time: %f", Time::game_time);

	static float scale = 1.0F;
	ImGui::PushItemWidth(75);
	if (ImGui::SliderFloat("Game Speed", &scale, 0.5F, 4.0F, "%.2f")) {
		Time::SetScaleTime(scale);
	}

	ImGui::SameLine();

	if (ImGui::Button("Reset Time"))
	{
		Time::SetScaleTime(1.f);
		scale = 1.f;
	}

	ImGui::SameLine();
	ImGui::Text("Time Since Start: %f", Time::time_since_start);
	ImGui::SameLine();
	ImGui::Text("Game Time: %f", Time::game_time);
	ImGui::End();
}

void ModuleUI::ResetImGui()
{
	OPTICK_EVENT();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	//
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.IniFilename = NULL;
	io.WantSaveIniSettings = false;

	ChangeStyle(App->window->style);

	ImGui_ImplOpenGL3_Init();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);


	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
}

void ModuleUI::ReportBug()
{
	App->OpenWebsite("https://github.com/VictorSegura99/Alien-GameEngine/issues");
}

void ModuleUI::ChangeStyle(const int& style_number)
{
	OPTICK_EVENT();

	switch (style_number) {
	case 0:
		ImGui::StyleColorsClassic();
		break;
	case 1:
		ImGui::StyleColorsDark();
		break;
	case 2:
		ImGui::StyleColorsLight();
		break;
	case 3:
		break;
	}

	ImGuiStyle& style =ImGui::GetStyle();
	style.MaxColumnSeparation;
	style.TitleSeparation;
	style.SubTitleSeparation;

}

void ModuleUI::ChangeEnableDemo()
{
	OPTICK_EVENT();

	show_demo_wndow = !show_demo_wndow;
}

void ModuleUI::CreateNewScriptPopUp()
{
	OPTICK_EVENT();

	static int type = 0;
	static char _name[MAX_PATH] = "Data Name";
	static bool _export = true;

	ImGui::OpenPopup("Create New Script");
	ImGui::SetNextWindowSize({ 320,155 });
	if (ImGui::BeginPopupModal("Create New Script", &creating_script, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
		ImGui::PushItemWidth(135);
		ImGui::SetCursorPosX(28);
		ImGui::Combo("Select Script Type", &type, "Script Type\0Class\0Struct\0");

		ImGui::Spacing();
		ImGui::SetCursorPosX(15);
		ImGui::InputText("Name", _name, MAX_PATH, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::SameLine();
		ImGui::Checkbox("Export Script", &_export);
		ImGui::Spacing();
		ImGui::SetCursorPosX(42);
		ImGui::Text("Header and Cpp will be genereated,");
		ImGui::SetCursorPosX(33);
		ImGui::Text("just add them as an existing element.");
		ImGui::Spacing();
		ImGui::SetCursorPosX(90);
		if (ImGui::Button("Create Script", { 130,25 })) {
			if (std::string(_name).find(" ") == std::string::npos && type != 0) {
				CreateScriptFile(type, _export, _name);
				type = 0;
				strcpy(_name, "Data Name");
				_export = true;
				creating_script = false;
			}
		}

		ImGui::EndPopup();
	}
	else {
		type = 0;
		strcpy(_name, "Data Name");
		_export = true;
	}
}

void ModuleUI::SetError()
{
	OPTICK_EVENT();

	if (!App->objects->errors) {
		App->objects->errors = true;
		panel_console->game_console = false;
		ImGui::SetWindowFocus(panel_console->GetName().data());
		if (App->camera->is_scene_focused) {
			ImGui::SetWindowFocus(panel_scene->GetName().data());
		}
		else if (panel_game->game_focused) {
			ImGui::SetWindowFocus(panel_game->GetName().data());
		}
	}
}

void ModuleUI::DeleteLayout(Layout* layout)
{
	OPTICK_EVENT();

	if (layout == active_layout) {
		layout->active = false;
		active_layout = layouts.front();
		active_layout->active = true;
	}
	std::vector<Layout*>::iterator item = layouts.begin();
	while (item != layouts.end()) {
		if (*item != nullptr && *item == layout) {
			App->file_system->Remove((*item)->path.data());
			need_to_save_layouts = true;
			delete* item;
			*item = nullptr;
			layouts.erase(item);
			break;
		}
		++item;
	}
}

void ModuleUI::InitPanels()
{
	OPTICK_EVENT();
	panel_about = new PanelAbout("About Alien Engine", panel_about_codes[0], panel_about_codes[1], panel_about_codes[2]);
	panel_project = new PanelProject("Panel Project", panel_project_codes[0], panel_project_codes[1], panel_project_codes[2]);
	panel_config = new PanelConfig("Configuration", panel_config_codes[0], panel_config_codes[1], panel_config_codes[2]);
	panel_console = new PanelConsole("Console", panel_console_codes[0], panel_console_codes[1], panel_console_codes[2]);
	panel_render = new PanelRender("Render Options", panel_render_codes[0], panel_render_codes[1], panel_render_codes[2]);
	panel_hierarchy = new PanelHierarchy("Panel Hierarchy", panel_hierarchy_codes[0], panel_hierarchy_codes[1], panel_hierarchy_codes[2]);
	panel_create_object = new PanelCreateObject("Create Object", panel_create_codes[0], panel_create_codes[1], panel_create_codes[2]);
	panel_inspector = new PanelInspector("Inspector", panel_inspector_codes[0], panel_inspector_codes[1], panel_inspector_codes[2]);
	panel_scene = new PanelScene("Scene", panel_scene_codes[0], panel_scene_codes[1], panel_scene_codes[2]);
	panel_scene_selector = new PanelSceneSelector("Save", panel_scene_selector_codes[0], panel_scene_selector_codes[1], panel_scene_selector_codes[2]);
	panel_text_editor = new PanelTextEditor("Text Editor", panel_text_edit_codes[0], panel_text_edit_codes[1], panel_text_edit_codes[2]);
	panel_layout = new PanelLayout("Layout Editor", panel_layout_codes[0], panel_layout_codes[1], panel_layout_codes[2]);
	panel_game = new PanelGame("Game", panel_game_codes[0], panel_game_codes[1], panel_game_codes[2]);
	panel_build = new PanelBuild("Build", panel_build_codes[0], panel_build_codes[1], panel_build_codes[2]);
	panel_animtimeline = new PanelAnimTimeline("Animation Timeline", panel_animtimeline_codes[0], panel_animtimeline_codes[1], panel_animtimeline_codes[2]);
	panel_animator = new PanelAnimator("Animator", panel_animator_codes[0], panel_animator_codes[1], panel_animator_codes[2]);
	panel_particles = new PanelParticleSystem("Particle System", panel_particles_codes[0], panel_particles_codes[1], panel_particles_codes[2]);
	panel_physics = new PanelPhysics("Physics", panel_physics_codes[0], panel_physics_codes[1], panel_physics_codes[2]);

	panels.push_back(panel_about);
	panels.push_back(panel_config);
	panels.push_back(panel_console);
	panels.push_back(panel_animtimeline);
	panels.push_back(panel_animator);
	panels.push_back(panel_project);
	panels.push_back(panel_render);
	panels.push_back(panel_hierarchy);
	panels.push_back(panel_create_object);
	panels.push_back(panel_inspector);
	panels.push_back(panel_game);
	panels.push_back(panel_scene);
	panels.push_back(panel_layout);
	panels.push_back(panel_scene_selector);
	panels.push_back(panel_text_editor);
	panels.push_back(panel_build);
	panels.push_back(panel_particles);
	panels.push_back(panel_physics);
}

void ModuleUI::UpdatePanels()
{
	OPTICK_EVENT();
	std::vector<Panel*>::iterator item = panels.begin();
	for (; item != panels.end(); ++item) {
		if (*item != nullptr) {
			if ((*item)->IsEnabled()) {
				(*item)->PanelLogic();
			}
		}
	}
}
                  
void ModuleUI::InitShortCuts()
{
	OPTICK_EVENT();

	shortcut_demo = App->shortcut_manager->AddShortCut("imGui Demo", shortcut_demo_codes[0], std::bind(&ModuleUI::ChangeEnableDemo, App->ui), shortcut_demo_codes[1], shortcut_demo_codes[2]);
	shortcut_report_bug = App->shortcut_manager->AddShortCut("Report Bug", shortcut_report_bug_codes[0], std::bind(&ModuleUI::ReportBug, App->ui), shortcut_report_bug_codes[1], shortcut_report_bug_codes[2]);
	shortcut_view_mesh = App->shortcut_manager->AddShortCut("Mesh View", shortcut_view_mesh_codes[0], std::bind(&ModuleObjects::ChangeViewMeshMode, App->objects), shortcut_view_mesh_codes[1], shortcut_view_mesh_codes[2]);
	shortcut_wireframe = App->shortcut_manager->AddShortCut("Wireframe Mode", shortcut_wireframe_codes[0], std::bind(&ModuleObjects::ChangeWireframeMode, App->objects), shortcut_wireframe_codes[1], shortcut_wireframe_codes[2]);
	shortcut_view_grid = App->shortcut_manager->AddShortCut("View Grid", shortcut_view_grid_codes[0], std::bind(&ModuleObjects::ChangeEnableGrid, App->objects), shortcut_view_grid_codes[1], shortcut_view_grid_codes[2]);
	shortcut_view_normal_vertex = App->shortcut_manager->AddShortCut("Normal Vertex", shortcut_view_normal_vertex_codes[0], std::bind(&ModuleObjects::ChangeEnableNormalVertex, App->objects), shortcut_view_normal_vertex_codes[1], shortcut_view_normal_vertex_codes[2]);
	shortcut_view_normal_face = App->shortcut_manager->AddShortCut("Normal Face", shortcut_view_normal_face_codes[0], std::bind(&ModuleObjects::ChangeEnableNormalFace, App->objects), shortcut_view_normal_face_codes[1], shortcut_view_normal_face_codes[2]);
	shortcut_new_scene = App->shortcut_manager->AddShortCut("New Scene", shortcut_new_scene_codes[0], std::bind(&PanelSceneSelector::OrganizeCreateNewScene, panel_scene_selector), shortcut_new_scene_codes[1], shortcut_new_scene_codes[2]);
	shortcut_save_scene_as_new = App->shortcut_manager->AddShortCut("Save Scene As...", shortcut_save_scene_as_new_codes[0], std::bind(&PanelSceneSelector::SaveSceneAsNew , panel_scene_selector), shortcut_save_scene_as_new_codes[1], shortcut_save_scene_as_new_codes[2]);
	shortcut_load_scene = App->shortcut_manager->AddShortCut("Load Scene", shortcut_load_scene_codes[0], std::bind(&PanelSceneSelector::OrganizeLoad, panel_scene_selector), shortcut_load_scene_codes[1], shortcut_load_scene_codes[2]);
	shortcut_outline = App->shortcut_manager->AddShortCut("Outline", shortcut_outline_codes[0], std::bind(&ModuleObjects::ChangeEnableOutline, App->objects), shortcut_outline_codes[1], shortcut_outline_codes[2]);
	shortcut_AABB = App->shortcut_manager->AddShortCut("AABB", shortcut_AABB_codes[0], std::bind(&ModuleObjects::ChangeEnableAABB, App->objects), shortcut_AABB_codes[1], shortcut_AABB_codes[2]);
	shortcut_OBB = App->shortcut_manager->AddShortCut("OBB", shortcut_OBB_codes[0], std::bind(&ModuleObjects::ChangeEnableOBB, App->objects), shortcut_OBB_codes[1], shortcut_OBB_codes[2]);
	shortcut_octree = App->shortcut_manager->AddShortCut("Octree", shortcut_octree_codes[0], std::bind(&ModuleObjects::ChangeEnableOctree, App->objects), shortcut_octree_codes[1], shortcut_octree_codes[2]);
	shortcut_cntrlZ = App->shortcut_manager->AddShortCut("Go Back", shortcut_cntrlZ_codes[0], ReturnZ::GoBackOneAction, shortcut_cntrlZ_codes[1], shortcut_cntrlZ_codes[2]);
	shortcut_cntrlY = App->shortcut_manager->AddShortCut("Go Forward", shortcut_cntrlY_codes[0], ReturnZ::GoFordwardOneAction, shortcut_cntrlY_codes[1], shortcut_cntrlY_codes[2]);
	shortcut_duplicate_object = App->shortcut_manager->AddShortCut("Duplicate Object", shortcut_duplicate_object_codes[0], std::bind(&ModuleObjects::DuplicateObjects, App->objects), shortcut_duplicate_object_codes[1], shortcut_duplicate_object_codes[2]);
	
	// OrderShortCuts must be called after all shortcuts have been created!! Victor read this...
	App->shortcut_manager->OrderShortCuts();
}

void ModuleUI::LoadActiveLayout()
{
	OPTICK_EVENT();

	std::vector<Layout*>::iterator item = layouts.begin();
	for (; item != layouts.end(); ++item) {
		if (*item != nullptr && (*item)->active) {
			std::vector<Panel*>::iterator panel = panels.begin();
			for (; panel != panels.end(); ++panel) {
				if (*panel != nullptr) {
					(*panel)->SetEnable((*item)->panels_enabled[panel - panels.begin()]);
				}
			}
			ImGui::LoadIniSettingsFromDisk((*item)->path.data());
			break;
		}
	}
}

void ModuleUI::HandleEvent(EventType eventType)
{
	OPTICK_EVENT();

	for each (Panel* p in panels)
	{
		switch (eventType)
		{
		case EventType::ON_PLAY:
			p->OnPlay();
			break;
		case EventType::ON_PAUSE:
			p->OnPause();
			break;
		case EventType::ON_STOP:
			p->OnStop();
			break;
		case EventType::ON_ASSET_SELECT:
			p->OnAssetSelect();
			break;
		case EventType::ON_ASSET_DELETE:
			p->OnAssetDelete();
			break;
		case EventType::ON_GO_SELECT:
			p->OnObjectSelect();
			break;
		case EventType::ON_GO_DELETE:
			p->OnObjectDelete();
			break;
		}
	}

	if (eventType == EventType::ON_UNLOAD_SCENE) {
		App->ui->panel_animtimeline->changed = true;
	}
}

void ModuleUI::FramerateRegister(float frames, float ms)
{
	OPTICK_EVENT();

	if (panel_config->IsEnabled())
		panel_config->FramerateInfo(frames,ms);
}

void ModuleUI::BackgroundDockspace()
{
	OPTICK_EVENT();

	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	// Seeting Docking to fit the window and preferences
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	/*if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;*/

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace BackGround", (bool*)0, window_flags);
	ImGui::PopStyleVar(3);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("Dockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();
}


Layout::Layout(const char* name)
{
	OPTICK_EVENT();

	this->name = std::string(name);

	bool done = false;
	path = std::string(CONFIGURATION_LAYOUTS_FOLDER) + name + std::string(".ini");
	int number = 1;
	while (!done) {
		if (!App->file_system->Exists(path.data())) {
			done = true;
		}
		else {
			path = std::string(CONFIGURATION_LAYOUTS_FOLDER) + name + std::to_string(number) + std::string(".ini");
			++number;
		}
	}

	ImGui::SaveIniSettingsToDisk(path.data());
}

Layout::~Layout()
{
	
}
