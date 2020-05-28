#pragma once

#include "Panel.h"
#include "Color.h"

class PanelCreateObject : public Panel {

public:

	PanelCreateObject(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~PanelCreateObject();
	void PanelLogic();

private:
	Color create_color{ 1,1,1 };
};

