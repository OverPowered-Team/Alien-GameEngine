#pragma once
#ifndef _RESOURCE_BONE_H_
#define _RESOURCE_BONE_H_

#include "Resource_.h"
#include "MathGeoLib/include/Math/float4x4.h"

class ResourceBone :
	public Resource
{
public:
	ResourceBone();
	~ResourceBone();

	std::string name;
	float4x4 matrix;

	uint num_weights;
	uint* vertex_ids;
	float* weights;

	std::string GetTypeString() const;
	void Load();
	void UnLoad();
};

#endif // !_RESOURCE_BONE_H_


