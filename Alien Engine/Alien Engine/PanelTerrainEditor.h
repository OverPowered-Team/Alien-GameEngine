#pragma once


#include "Panel.h"
#include "ResourceTexture.h"

class PanelTerrainEditor : public Panel {

public:

	PanelTerrainEditor(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~PanelTerrainEditor();

	void PanelLogic();
	void SetHeigthMapTexture(ResourceTexture* tex);
	

private:

	

public:

	std::string file_name;

private:

	ResourceTexture* heigthmapTexture = nullptr;
	bool is_focused = false;
};