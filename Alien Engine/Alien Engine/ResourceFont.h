#pragma once

#include "Resource_.h"
#include "MathGeoLib/include/Math/float2.h"
#include <map>

struct FontImportSettings {
	std::string fontPath;
	std::vector<uint> sizes;
};

struct Character {
	uint    TextureID;  // ID handle of the glyph texture
	float2	Size;       // Size of glyph
	float2	Bearing;    // Offset from baseline to left/top of glyph
	uint    Advance;    // Offset to advance to next glyph
};

struct ResourceFontData {
	uint fontSize;
	uint maxCharHeight;
	std::map<char, Character> charactersMap;
	std::vector<uint8_t*> fontBuffer;
};

class ResourceFont : public Resource {

public:
	ResourceFont() { type = ResourceType::RESOURCE_TEXTURE; }
	
	Resource* ImportFile(const char* file);
	/*ResourceFont(const char* path, const uint& id, const uint& width, const uint& height);
	ResourceFont(const char* path);
	virtual ~ResourceFont();*/

	/*bool CreateMetaData(const u64& force_id = 0);
	bool LoadMemory();
	void FreeMemory();
	bool ReadBaseInfo(const char* assets_path);
	void ReadLibrary(const char* meta_data);
	bool DeleteMetaData();*/

public:
	FontImportSettings importSettings;
	ResourceFontData fontData;
};
