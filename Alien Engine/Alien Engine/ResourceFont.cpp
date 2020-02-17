#include "Application.h"
#include "ResourceFont.h"
#include "ModuleFileSystem.h"
#include "FreeType/include/ft2build.h"

#pragma comment(lib, "Freetype/libx86/freetype.lib")

Resource* ResourceFont::ImportFile(const char* file)
{
	if(!App->file_system->Exists(file))
		return nullptr;

	//Search for met associated

	//Check for meta file
	
}
