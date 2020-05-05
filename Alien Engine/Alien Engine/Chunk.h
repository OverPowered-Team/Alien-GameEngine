#pragma once
#include "Globals.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "PanelTerrainEditor.h"
class Chunk
{
public:

	Chunk();
	Chunk(PanelTerrainEditor* owner);
	~Chunk();

	int GetBufferID();
	int GetNumIndices();
	const uint* GetIndices();

	void GenerateBuffer();
	void AllocateIndex(const uint& i);

	void UpdateAABB();
	void DeleteIndices();

	void Render();

	AABB GetAABB();
	void SetAABB(float3 minPoint, float3 MaxPoint);

private:
	uint* indices = nullptr;
	uint num_indices;
	int index_id;
	uint chunck_memory = 0;
	AABB bbox;
	PanelTerrainEditor* owner = nullptr;
};