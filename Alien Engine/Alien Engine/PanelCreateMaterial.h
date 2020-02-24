#pragma once

#include "Panel.h"

class PanelCreateMaterial : public Panel {

public:

	PanelCreateMaterial(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	~PanelCreateMaterial();
	void PanelLogic();
	void SetUniforms();

private:
	Color create_color{ 1,1,1 };
	char path[MAX_PATH] = "path";
	char uniform_name[MAX_PATH] = "uniform name";
	char* uniform_type_char = "no type";
};

