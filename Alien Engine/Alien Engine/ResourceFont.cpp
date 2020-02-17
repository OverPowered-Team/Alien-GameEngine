#include "Application.h"
#include "ResourceFont.h"
#include "ModuleFileSystem.h"
#include "ModuleImporter.h"
#include "FreeType/include/freetype/freetype.h"
#include "glew/include/glew.h"

ResourceFont::ResourceFont(ResourceFontData fontData):Resource(), fontData(fontData)
{
	type = ResourceType::RESOURCE_FONT;
}

Resource* ResourceFont::ImportFile(const char* file)
{
	if(!App->file_system->Exists(file))
		return nullptr;

	//Search for met associated

	//Check for meta file
	
}

ResourceFont* ResourceFont::ImportFontBySize(const char* file, uint size)
{
	ResourceFont* font = nullptr;
	uint maxHeight = 0;
	std::map<char, Character> charactersBitmap;

	FT_Face face;
	if (FT_New_Face(App->importer->library, file, 0, &face))
	{
		LOG_ENGINE("The font file can't be opened, read or unsopported format!");
	}
	else
	{
		ResourceFontData fontData;
		FT_Set_Pixel_Sizes(face, 0, size);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for (uint c = 32; c < 128; c++) {
			//Load character gylph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				LOG_ENGINE("Failed Glyph from freetype the character %c", c);
				continue;
			}
			uint texture = LoadTextureCharacter(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);

			// Now store character in the char map
			Character character = {
				texture,
				float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x / 64
			};
			charactersBitmap.insert(std::pair<char, Character>(c, character));
			if (face->glyph->bitmap.rows > maxHeight)
				maxHeight = face->glyph->bitmap.rows;

			//Save buffer for next loads
			uint bufferSize = face->glyph->bitmap.width * face->glyph->bitmap.rows;
			uint8_t* characterBuffer = new uint8_t[bufferSize];
			uint bytes = sizeof(uint8_t) * bufferSize;
			memcpy(characterBuffer, (uint8_t*)face->glyph->bitmap.buffer, bytes);

			fontData.fontBuffer.push_back(characterBuffer);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		FT_Done_Face(face);

		fontData.charactersMap = charactersBitmap;
		fontData.fontSize = size;
		fontData.maxCharHeight = maxHeight;

		font = new ResourceFont(fontData);
		font->path = file;
		font->name = App->file_system->GetBaseFileName(file);
		//font->exported_file;
		//font->meta_data_path;
	}

	return font;
}

ResourceFont* ResourceFont::LoadFile(const char* file)
{
	ResourceFont* res = nullptr;

	char* buffer;
	uint size = App->file_system->Load(file, &buffer);
	if (size > 0) {
		//Get UID

		ResourceFontData fontData;
		char* cursor = (char*)buffer;

		uint bytes = sizeof(uint);
		memcpy(&fontData.fontSize, cursor, bytes);
		cursor += bytes;

		memcpy(&fontData.maxCharHeight, cursor, bytes);
		cursor += bytes;

		uint charactersSize = 0;
		bytes = sizeof(uint);
		memcpy(&charactersSize, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(Character);
		for (uint i = 0; i < charactersSize; ++i)
		{
			Character character;
			memcpy(&character, cursor, bytes);
			fontData.charactersMap.insert(std::pair<char, Character>(i + 32, character));

			cursor += bytes;
		}

		fontData.fontBuffer.resize(charactersSize);
		for (uint i = 0; i < charactersSize; ++i)
		{
			uint width = fontData.charactersMap[i + 32].size.x;
			uint height = fontData.charactersMap[i + 32].size.y;

			bytes = sizeof(uint8_t) * width * height;
			uint8_t* buffer = new uint8_t[width * height];
			memcpy(buffer, cursor, bytes);
			fontData.fontBuffer[i] = buffer;
			fontData.charactersMap[i + 32].textureID = ResourceFont::LoadTextureCharacter(width, height, fontData.fontBuffer[i]);

			cursor += bytes;
		}
	}

	return res;
}

uint ResourceFont::LoadTextureCharacter(uint width, uint height, uchar* buffer)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);

	// Set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	return texture;
}

uint ResourceFont::SaveFile(ResourceFontData& fontData)
{
	uint sizeBuffer = 0;
	for (uint i = 0; i < fontData.charactersMap.size(); ++i)
	{

	}

	return true;
}
