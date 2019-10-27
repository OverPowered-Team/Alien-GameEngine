#include "ModuleResources.h"
#include "SDL/include/SDL_assert.h"
#include "Resource_.h"
#include "ResourceMesh.h"
#include "ResourceModel.h"
#include "ModuleImporter.h"
#include "Application.h"
#include "ResourceTexture.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
	name = "Resources";
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Start()
{
	ReadAllMetaData();

	// Load Icons
	icons.jpg_file = App->importer->LoadTextureFile("Library/Textures/EngineTextures/icon_jpg.png");
	icons.jpg_file->is_custom = false;

	icons.png_file = App->importer->LoadTextureFile("Library/Textures/EngineTextures/icon_png.png");
	icons.png_file->is_custom = false;

	icons.dds_file = App->importer->LoadTextureFile("Library/Textures/EngineTextures/icon_dds.png");
	icons.dds_file->is_custom = false;

	icons.folder = App->importer->LoadTextureFile("Library/Textures/EngineTextures/icon_folder.png");
	icons.folder->is_custom = false;
	
	icons.model = App->importer->LoadTextureFile("Library/Textures/EngineTextures/icon_model.png");
	icons.model->is_custom = false;

	return true;
}

update_status ModuleResources::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleResources::CleanUp()
{

	// TODO: clean up resources

	return true;
}

void ModuleResources::AddResource(Resource* resource)
{
	SDL_assert((uint)ResourceType::RESOURECE_MAX == 3); // add new type

	if (resource != nullptr) {
		switch (resource->GetType())
		{
		case ResourceType::RESOURCE_MODEL:
			if (std::find(resource_models.begin(), resource_models.end(), resource) == resource_models.end())
				resource_models.push_back((ResourceModel*)resource);
			break;
		case ResourceType::RESOURCE_MESH:
			if (std::find(resource_meshes.begin(), resource_meshes.end(), resource) == resource_meshes.end())
				resource_meshes.push_back((ResourceMesh*)resource);
			break;
		case ResourceType::RESOURCE_TEXTURE:
			if (std::find(resource_textures.begin(), resource_textures.end(), resource) == resource_textures.end())
				resource_textures.push_back((ResourceTexture*)resource);
			break;
		default:
			break;
		}
	}

}

void ModuleResources::CreateNewModelInstanceOf(const char* path)
{
	std::vector<ResourceModel*>::iterator item = resource_models.begin();
	for (; item != resource_models.end(); ++item) {
		if (*item != nullptr) {
			if (App->StringCmp((*item)->GetLibraryPath(), path)) {
				(*item)->ConvertToGameObjects();
				break;
			}
		}
	}
}

void ModuleResources::ReadAllMetaData()
{
	std::vector<std::string> files;
	std::vector<std::string> directories;

	App->file_system->DiscoverFiles(LIBRARY_MODELS_FOLDER, files, directories);

	for (uint i = 0; i < files.size(); ++i) {
		ResourceModel* model = new ResourceModel();
		model->ReadMetaData(std::string(LIBRARY_MODELS_FOLDER + files[i]).data());
	}

	// TODO: Textures
}

