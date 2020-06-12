#pragma once

#include "Resource_.h"
#include <vector>

class JSONfilepack;
class ResourceTexture : public Resource {

public:

	ResourceTexture() { type = ResourceType::RESOURCE_TEXTURE; }
	ResourceTexture(const char* path, const uint& id, const uint& width, const uint& height);
	ResourceTexture(const char* path);
	virtual ~ResourceTexture();

	bool CreateMetaData(const u64& force_id = 0);
	bool LoadMemory();
	void FreeMemory();
	bool ReadBaseInfo(const char* assets_path);
	void ReadLibrary(const char* meta_data);
	bool DeleteMetaData();

	void SaveTextureValues(JSONfilepack* file);
	void ReadTextureValues(JSONfilepack* file);

	void DisplayTextureOnInspector();

	void IncreaseReferences() override;


public:

	bool is_custom = true;

	uint height = 0;
	uint width = 0;
	uint id = 0;
	float alpha_test = 0.1f;

	int wrap_type = 0;
	int texture_filter = 0;

	bool ignore_next_increase = false;

};