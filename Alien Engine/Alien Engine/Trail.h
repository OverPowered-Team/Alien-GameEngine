#pragma once

#include "Globals.h"

class __declspec(dllexport) Trail
{
	friend class ComponentTrail;
public:

	Trail();
	~Trail();

private:

	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool Draw();
	
public:
	
	// Vertex Array Object
	uint vao = 0u;

	// Buffers id
	uint id_index = 0;
	uint id_vertex = 0;
	uint id_uv = 0;

};