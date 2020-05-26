#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/OBB.h"
#include "Color.h"
#include "ResourceTexture.h"
#include "ResourceMaterial.h"
#define CAP(n, _min, _max) ((n <= _min) ? n=_min : (n >= _max) ? n=_max : n=n)
#define CHUNK_W 64
#define CHUNK_H 64

#include "Chunk.h"
#include <map>
class Chunk;

enum class TERRAIN_MODE
{
	MODE_HEIGHTMAP,
	MODE_SCULPT,
	MODE_PAINT,
	MODE_OBJECT,

	MODE_UNKNOWN = -1
};
class __declspec(dllexport) ComponentTerrain : public Component {
	//friend class Chunck;
	
public:
	ComponentTerrain(GameObject* attach);
	virtual ~ComponentTerrain();
	
	bool DrawInspector();

	void SetHeigthMapTexture(ResourceTexture* tex);

	uint GetHeightMapID();
	float2 GetHeightMapSize();
	const float3* GetVertices();
	void SetUniforms(ResourceMaterial* resource_material);
	void SetTerrainMode(TERRAIN_MODE mode);


	void HeighMapMode();
	void ScuptMode();
	void PaintMode();
	void ObjectMode();


private:

	void GenerateHeightMap(const char* path, int gaussian);
	void SetPixelData(const char* path);
	void AllocateHeightMapBuffer();
	void GaussianBlur(int iterations);


	void GenerateTerrainMesh();

	void GenerateVertices();
	void GenerateIndices();
	void GenerateNormals();
	void GenerateTexCoords();

	void AllocateMeshBuffers();
	void DeallocateMeshBuffers();

	void DrawGame();
	void DrawScene();

	void DrawTerrain();
public:

	std::string file_name;

public:

	int m_Width;
	int m_Height;
	std::vector <float3> m_data;

	ResourceTexture* heigthmapTexture = nullptr;
	bool is_focused = false;
	uint heightmap_id = 0;


	float* terrainData = nullptr;
	float* terrainDataHeight = nullptr;
	float maxHeight = 100.0f;

	float texScale = 0.03f;

	//vao
	uint vao = 0u;

	//vertex data
	float3* vertices = nullptr;
	uint num_vertices;
	uint vertex_id;

	//normal data
	float3* normals = nullptr;
	uint num_normals;
	uint normals_id;

	//texCoords data
	float3* uvs = nullptr;
	float3* scaled_uvs = nullptr;
	uint num_uvs;
	uint uv_id;
	uint scaled_uv_id;

	bool chunkDraw = false;

	//Chunk test;
	std::map<int, std::map<int, Chunk>> chunks;

	ComponentMaterial* material = nullptr;
	int animate = 0;
	TERRAIN_MODE mode = TERRAIN_MODE::MODE_HEIGHTMAP;
	//
};
