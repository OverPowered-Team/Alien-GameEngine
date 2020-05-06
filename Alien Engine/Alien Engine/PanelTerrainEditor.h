#pragma once


#include "Panel.h"
#include "ResourceTexture.h"
//#include "Chunk.h"
#include <map>

#define CAP(n, _min, _max) ((n <= _min) ? n=_min : (n >= _max) ? n=_max : n=n)
#define CHUNK_W 64
#define CHUNK_H 64

//class Chunk;

class PanelTerrainEditor : public Panel {

public:

	PanelTerrainEditor(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~PanelTerrainEditor();

	void PanelLogic();
	//void SetHeigthMapTexture(ResourceTexture* tex);

	//uint GetHeightMapID();
	//float2 GetHeightMapSize();
	//const float3* GetVertices();

private:

	//void GenerateHeightMap(const char* path, int gaussian);
	//void SetPixelData(const char* path);
	//void AllocateHeightMapBuffer();
	//void GaussianBlur(int iterations);


	//void GenerateTerrainMesh();
	//
	//void GenerateVertices();
	//void GenerateIndices();
	//void GenerateNormals();
	//void GenerateTexCoords();

	//void AllocateMeshBuffers();
	//void DeallocateMeshBuffers();

public:

	std::string file_name;

private:

	//int m_Width;
	//int m_Height;
	//std::vector <float3> m_data;

	//ResourceTexture* heigthmapTexture = nullptr;
	//bool is_focused = false;
	//uint heightmap_id = 0;


	//float3* terrainData = nullptr;
	//float3* terrainDataHeight = nullptr;
	//float maxHeight = 100.0f;

	//float texScale = 0.03f;

	////vertex data
	//float3* vertices = nullptr;
	//uint num_vertices;
	//uint vertex_id;

	////normal data
	//float3* normals = nullptr;
	//uint num_normals;
	//uint normals_id;

	////texCoords data
	//float2* uvs = nullptr;
	//float2* scaled_uvs = nullptr;
	//uint num_uvs;
	//uint uv_id;
	//uint scaled_uv_id;
	//std::map<int, std::map<int, Chunk>> chunks;
};