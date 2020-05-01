#ifndef __GBUFFER_H__
#define __GBUFFER_H__

#include "glew/include/glew.h"

#include "Globals.h"

class ResourceTexture;

class GBuffer
{
public:
	GBuffer();
	~GBuffer();

	void GenerateGBuffer();
	void DestroyGBuffer(); 

	void BeginGBuffer(); 
	void EndGBuffer();

	void UpdateTextures();
	void SetSize(float width, float height);

private:
	float width = 0.f;
	float height = 0.f; 

	uint gBuffer = 0;

	uint gPosition = 0u;
	uint gNormals = 0u;
	uint gColorSpec = 0u;

	uint attachments[3] = { 0 };
	bool to_update = true; 
};

#endif
