#pragma once

#include "Panel.h"

class PanelConsole : public Panel {

public:

	PanelConsole(const std::string& panel_name, const std::vector<SDL_Scancode>& shortcuts);
	virtual ~PanelConsole();

	void PanelLogic();
	void OnPanelDesactive();

private:

	bool scroll_x = true;
	bool scroll_y = true;

};

