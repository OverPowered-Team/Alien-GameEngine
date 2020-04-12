#pragma once

#include "Panel.h"

class PanelNavigation : public Panel {

public:

	PanelNavigation(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~PanelNavigation();

	void PanelLogic();

private:
	void OnPanelDesactive();

	void ShowObjectTab();
	void ShowAreaTab();
	void ShowBakeTab();
	const std::string GetFloatPrecisionString(const float value, const int precision = 2.0f) const;

public:

	bool panel_nav_rendered = false;
};