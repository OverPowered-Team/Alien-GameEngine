#pragma once


#include "Panel.h"
#include "ResourceTexture.h"

class PanelTerrainEditor : public Panel {

public:

	PanelTerrainEditor(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~PanelTerrainEditor();

	void PanelLogic();
	void SetHeigthMapTexture(ResourceTexture* tex);

	uint GetHeightMapID();
	float2 GetHeightMapSize();

private:

	void GenerateHeightMap(const char* path, int gaussian);
	void SetPixelData(const char* path);
	void GenerateHeightMapBuffer();
	void GaussianBlur(int iterations);

	

public:

	std::string file_name;

private:

	int m_Width;
	int m_Height;
	std::vector <float3> m_data;

	ResourceTexture* heigthmapTexture = nullptr;
	bool is_focused = false;
	uint heightmap_id = 0;
};