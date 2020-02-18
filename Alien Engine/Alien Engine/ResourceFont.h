#pragma once

#include "Resource_.h"
#include "MathGeoLib/include/Math/float2.h"
#include <map>

typedef unsigned char uchar;

struct FontImportSettings {
	std::string fontPath;
	std::vector<uint> sizes;
};

struct Character {
	uint    textureID;  // ID handle of the glyph texture
	float2	size;       // Size of glyph
	float2	bearing;    // Offset from baseline to left/top of glyph
	uint    advance;    // Offset to advance to next glyph
};

struct ResourceFontData {
	uint fontSize;
	uint maxCharHeight;
	std::map<char, Character> charactersMap;
	std::vector<uint8_t*> fontBuffer;
};

class ResourceFont : public Resource {

public:
	ResourceFont(ResourceFontData fontData);
	
	Resource* ImportFile(const char* file);
	static ResourceFont* ImportFontBySize(const char* file, uint size);
	ResourceFont* LoadFile(const char* file);

private:
	static uint LoadTextureCharacter(uint width, uint height, uchar* buffer);
	static uint SaveFile(ResourceFontData& fontData, const char* exported_path);
	/*ResourceFont(const char* path, const uint& id, const uint& width, const uint& height);
	ResourceFont(const char* path);
	virtual ~ResourceFont();*/

	/*bool LoadMemory();
	void FreeMemory();
	bool ReadBaseInfo(const char* assets_path);
	void ReadLibrary(const char* meta_data);
	bool DeleteMetaData();*/

public:
	FontImportSettings importSettings;
	ResourceFontData fontData;
};
