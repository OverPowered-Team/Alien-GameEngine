#pragma once

#include "Panel.h"

class GameObject;

class PanelSceneSelector : public Panel {

public:

	friend class ModuleUI;

	enum class SceneSelectorState {
		SAVE_SCENE,
		SAVE_AS_NEW,
		LOAD_SCENE,
		CREATE_NEW_SCENE,

		NONE
	};

	PanelSceneSelector(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~PanelSceneSelector();

	void PanelLogic();

	void OrganizeSave(const SceneSelectorState& state);

	void LoadSceneWithPath(const char* path);

private:

	void SaveSceneAsNew();
	void LoadScene();
	void CreateNewScene();
	void MenuSaveCurrentScene();
	void OrganizeSaveScene();
	void OrganizeCreateNewScene();
	void OrganizeLoad();

public:

	GameObject* force_save = nullptr;

private:

	bool menu_save_current = false;
	bool create_new = false;
	bool load = false;

	std::string scene_to_load;
};

