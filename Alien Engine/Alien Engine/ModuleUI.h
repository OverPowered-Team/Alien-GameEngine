#pragma once
#include "Module.h"
#include "Globals.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_sdl.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include "Panel.h"
#include "Time.h"
#include "TextEdit/TextEditor.h"
#include <utility>

class GameObject;
class Panel;
class PanelConfig;
class PanelAbout;
class PanelConsole;
class PanelCreateObject;
class PanelRender;
class PanelHierarchy;
class PanelInspector;
class PanelScene;
class PanelLayout;
class PanelProject;
class PanelSceneSelector;
class PanelGame;
class PanelBuild;
class PanelTextEditor;
class PanelAnimTimeline;
class PanelAnimator;
class PanelParticleSystem;
class PanelPhysics;
struct ShortCut;

struct Layout {

	Layout() {}
	Layout(const char* name);
	~Layout();

	std::string name;
	std::string path;

	bool active = false;
	std::vector<bool> panels_enabled;

	uint number = 0;
};


class ModuleUI : public Module
{
public:
	ModuleUI(bool start_enabled = true);
	~ModuleUI();

	bool Start();
	update_status PreUpdate(float dt);
	bool CleanUp();

	// config
	void LoadConfig(JSONfilepack*& config);
	void SaveConfig(JSONfilepack*& config);

	// draw UI
	void MainMenuBar();
	void SecondMenuBar();
	void Draw();

	// reset imgui
	void ResetImGui();

	// callers
	void ReportBug();
	void ChangeEnableDemo();

	void CreateNewScriptPopUp();

	void SetError();

	// style
	void ChangeStyle(const int& style_number);

	// layout settings
	void DeleteLayout(Layout* layout); // delete x layout
	void SaveLayout(Layout* layout, bool is_new = true); // save x layout
	void LoadActiveLayout(); // charge the active layout //

	void HandleEvent(EventType eventType);

private:

	// panels
	void InitPanels();
	void UpdatePanels();

	// shortcuts
	void InitShortCuts();

	// more layouts 
	void LoadLayouts(); // read all layouts with json //
	void SaveAllLayouts(); // save all layout info again to json
	void SaveLayoutsActive(); // when closing, save which layout was active

	void CreateScriptFile(const int& type, bool to_export, const char* name);

public:

	// framerate
	void FramerateRegister(float frames, float ms);

	// big window to dock everything
	void BackgroundDockspace();

	



private:
	TextEditor edit;
	// demo
	bool show_demo_wndow = false;

	
	// Panels
	std::vector<Panel*> panels;

	PanelAbout* panel_about = nullptr;
	PanelRender* panel_render = nullptr;
	PanelInspector* panel_inspector = nullptr;
	PanelBuild* panel_build = nullptr;
	PanelLayout* panel_layout = nullptr;
	PanelParticleSystem* panel_particles = nullptr;

	// ShortCuts
	ShortCut* shortcut_demo = nullptr;
	ShortCut* shortcut_report_bug = nullptr;
	ShortCut* shortcut_wireframe = nullptr;
	ShortCut* shortcut_view_mesh = nullptr;
	ShortCut* shortcut_view_grid = nullptr;
	ShortCut* shortcut_view_normal_vertex = nullptr;
	ShortCut* shortcut_view_normal_face = nullptr;
	ShortCut* shortcut_new_scene = nullptr;
	ShortCut* shortcut_save_scene_as_new = nullptr;
	ShortCut* shortcut_load_scene = nullptr;
	ShortCut* shortcut_outline = nullptr;
	ShortCut* shortcut_AABB = nullptr;
	ShortCut* shortcut_OBB = nullptr;
	ShortCut* shortcut_octree = nullptr;
	ShortCut* shortcut_cntrlZ = nullptr;
	ShortCut* shortcut_cntrlY = nullptr;
	ShortCut* shortcut_duplicate_object = nullptr;

	SDL_Scancode panel_config_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_project_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_build_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_layout_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_scene_selector_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_about_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_hierarchy_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_create_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_create_materials[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_inspector_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_console_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_text_edit_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_render_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_scene_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_game_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_animtimeline_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_animator_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_particles_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode panel_physics_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };

	SDL_Scancode shortcut_demo_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_report_bug_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_wireframe_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_view_mesh_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_view_grid_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_view_normal_vertex_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_view_normal_face_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_new_scene_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_save_scene_as_new_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_load_scene_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_outline_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_AABB_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_OBB_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_octree_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_cntrlZ_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_cntrlY_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };
	SDL_Scancode shortcut_duplicate_object_codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN };

	std::pair<bool, Time::GameState> change_game_state;

public:
	// public panels
	PanelConfig* panel_config = nullptr;
	PanelConsole* panel_console = nullptr;
	PanelCreateObject* panel_create_object = nullptr;
	PanelProject* panel_project = nullptr;
	PanelSceneSelector* panel_scene_selector = nullptr;
	PanelScene* panel_scene = nullptr;
	PanelGame* panel_game = nullptr;
	PanelHierarchy* panel_hierarchy = nullptr;
	PanelTextEditor* panel_text_editor = nullptr;
	PanelAnimTimeline* panel_animtimeline = nullptr;
	PanelAnimator* panel_animator = nullptr;
	PanelPhysics* panel_physics = nullptr;
	bool update_shortcut = true;


	// layouts
	std::vector<Layout*> layouts;
	Layout* active_layout = nullptr;
	bool need_to_save_layouts = false;

	bool creating_script = false;
	const char* actual_name="";
};

