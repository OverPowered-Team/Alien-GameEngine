#include "Chunk.h"
#include "PanelTerrainEditor.h"
//#include "ComponentTerrain.h"
#include "MathGeoLib/include/Math/float4.h"
Chunk::Chunk()
{
	bbox.SetNegativeInfinity();
}

Chunk::Chunk(ComponentTerrain* owner) : owner(owner)
{
	bbox.SetNegativeInfinity();
}

Chunk::~Chunk()
{
	glDeleteBuffers(1, (GLuint*)&index_id);
	index_id = 0;
}


void Chunk::GenerateBuffer()
{
	if (num_indices <= 0)
		return;

	glGenBuffers(1, (GLuint*) & (index_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)* num_indices, indices, GL_STATIC_DRAW);
}

void Chunk::AllocateIndex(const uint& i)
{
	if (num_indices + 1 >= chunck_memory)
	{
		chunck_memory = num_indices + 64;
		uint* tmp = new uint[chunck_memory];
		memcpy(tmp, indices, sizeof(uint) * num_indices);
		RELEASE_ARRAY(indices);
		indices = tmp;
	}

	indices[num_indices] = i;
	num_indices++;
}

void Chunk::UpdateAABB()
{
	bbox.SetNegativeInfinity();
	const float3* vertex = owner->GetVertices();

	for (int n = 0; n < num_indices; n++)
		bbox.Enclose(vertex[indices[n]]);
	
}

void Chunk::DeleteIndices()
{
	RELEASE_ARRAY(indices);
	num_indices = 0;
}

void Chunk::Render()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1.5f);
	
	glColor4fv(float4(0.674, 0.784, 0.886, 1.0f).ptr());
	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(bbox.minPoint.x, bbox.minPoint.y, bbox.maxPoint.z);
	glVertex3f(bbox.maxPoint.x, bbox.minPoint.y, bbox.maxPoint.z);
	glVertex3f(bbox.maxPoint.x, bbox.maxPoint.y, bbox.maxPoint.z);
	glVertex3f(bbox.minPoint.x, bbox.maxPoint.y, bbox.maxPoint.z);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(bbox.maxPoint.x, bbox.minPoint.y, bbox.minPoint.z);
	glVertex3f(bbox.minPoint.x, bbox.minPoint.y, bbox.minPoint.z);
	glVertex3f(bbox.minPoint.x, bbox.maxPoint.y, bbox.minPoint.z);
	glVertex3f(bbox.maxPoint.x, bbox.maxPoint.y, bbox.minPoint.z);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(bbox.maxPoint.x, bbox.minPoint.y, bbox.maxPoint.z);
	glVertex3f(bbox.maxPoint.x, bbox.minPoint.y, bbox.minPoint.z);
	glVertex3f(bbox.maxPoint.x, bbox.maxPoint.y, bbox.minPoint.z);
	glVertex3f(bbox.maxPoint.x, bbox.maxPoint.y, bbox.maxPoint.z);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(bbox.minPoint.x, bbox.minPoint.y, bbox.minPoint.z);
	glVertex3f(bbox.minPoint.x, bbox.minPoint.y, bbox.maxPoint.z);
	glVertex3f(bbox.minPoint.x, bbox.maxPoint.y, bbox.maxPoint.z);
	glVertex3f(bbox.minPoint.x, bbox.maxPoint.y, bbox.minPoint.z);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(bbox.minPoint.x, bbox.maxPoint.y, bbox.maxPoint.z);
	glVertex3f(bbox.maxPoint.x, bbox.maxPoint.y, bbox.maxPoint.z);
	glVertex3f(bbox.maxPoint.x, bbox.maxPoint.y, bbox.minPoint.z);
	glVertex3f(bbox.minPoint.x, bbox.maxPoint.y, bbox.minPoint.z);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(bbox.minPoint.x, bbox.minPoint.y, bbox.minPoint.z);
	glVertex3f(bbox.maxPoint.x, bbox.minPoint.y, bbox.minPoint.z);
	glVertex3f(bbox.maxPoint.x, bbox.minPoint.y, bbox.maxPoint.z);
	glVertex3f(bbox.minPoint.x, bbox.minPoint.y, bbox.maxPoint.z);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor4fv(float4(1.f, 1.f, 1.f, 1.f).ptr());
}



void Chunk::SetAABB(float3 minPoint, float3 MaxPoint)
{
	bbox.minPoint = minPoint;
	bbox.maxPoint = MaxPoint;
}

int Chunk::GetBufferID()
{
	return index_id;
}

int Chunk::GetNumIndices()
{
	return num_indices;
}

const uint* Chunk::GetIndices()
{
	return indices;
}

AABB Chunk::GetAABB()
{
	return bbox;
}