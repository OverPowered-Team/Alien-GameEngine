#pragma once

#include "Panel.h"

class PanelConsole : public Panel {

public:

	PanelConsole(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~PanelConsole();

	void PanelLogic();
	void OnPanelDesactive();

public:

	bool clear_on_play = true;
	bool game_console = false;
private:
	bool collapse = true;

	bool scroll_x = false;
	bool scroll_y = true;

};

