#include "ResourceTexture.h"
#include "ModuleResources.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"
#include "ModuleImporter.h"
#include "mmgr/mmgr.h"

ResourceTexture::ResourceTexture(const char* path, const uint& id, const uint& width, const uint& height) : Resource()
{
	this->path = std::string(path);
	this->id = id;
	this->width = width;
	this->height = height;
	name = App->file_system->GetBaseFileName(path);
	type = ResourceType::RESOURCE_TEXTURE;

}

ResourceTexture::ResourceTexture(const char* path)
{
	name = App->file_system->GetBaseFileName(path); 
	this->path = std::string(path); 
	type = ResourceType::RESOURCE_TEXTURE;

}

ResourceTexture::~ResourceTexture()
{
	glDeleteTextures(1, &id);
}

bool ResourceTexture::CreateMetaData(const u64& force_id)
{
	bool ret = false;

	if (ilLoadImage(path.data())) {
		if (force_id == 0)
			ID = App->resources->GetRandomID();
		else
			ID = force_id;

		iluFlipImage();

		std::string alien_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien").data();

		JSON_Value* alien_value = json_value_init_object();
		JSON_Object* alien_object = json_value_get_object(alien_value);
		json_serialize_to_file_pretty(alien_value, alien_path.data());

		if (alien_value != nullptr && alien_object != nullptr) {
			JSONfilepack* alien = new JSONfilepack(alien_path.data(), alien_object, alien_value);
			alien->StartSave();
			alien->SetString("Meta.ID", std::to_string(ID).data());
			SaveTextureValues(alien);
			alien->FinishSave();
			delete alien;
		}
		//for library
		string new_meta = std::string(LIBRARY_TEXTURES_FOLDER + std::to_string(ID) + ".meta");
		std::string ext;
		App->file_system->SplitFilePath(alien_path.data(), nullptr, nullptr, &ext);
		if (App->StringCmp(ext.data(), "dds")) {
			App->file_system->Copy(alien_path.data(), new_meta.data());
		}

		meta_data_path = std::string(LIBRARY_TEXTURES_FOLDER + std::to_string(ID) + ".dds");
		std::string ext2;
		App->file_system->SplitFilePath(path.data(), nullptr, nullptr, &ext);
		if (App->StringCmp(ext2.data(), "dds")) {
			App->file_system->Copy(path.data(), meta_data_path.data());
			id = ilutGLBindTexImage();
			is_custom = true;
			width = ilGetInteger(IL_IMAGE_WIDTH);
			height = ilGetInteger(IL_IMAGE_HEIGHT);
		}
		else {
			iluFlipImage();
			ILuint image_size;
			ILubyte* image_data;
			ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
			image_size = ilSaveL(IL_DDS, NULL, 0); 
			if (image_size > 0) {
				image_data = new ILubyte[image_size]; 
				if (ilSaveL(IL_DDS, image_data, image_size) > 0) {
					std::string output;
					App->file_system->SaveUnique(output, image_data, image_size, LIBRARY_TEXTURES_FOLDER, std::to_string(ID).data(), ".dds");
				}
				iluFlipImage();
				id = ilutGLBindTexImage();
				is_custom = true;
				width = ilGetInteger(IL_IMAGE_WIDTH);
				height = ilGetInteger(IL_IMAGE_HEIGHT);
				
				RELEASE_ARRAY(image_data);
			}
		}

		ret = true;
		glBindTexture(GL_TEXTURE_2D, 0);
		App->resources->AddResource(this);
		FreeMemory();
	}
	else {
		delete this;
		ret = false;
	}

	return ret;
}

bool ResourceTexture::LoadMemory()
{
	return App->importer->LoadTextureToResource(meta_data_path.data(), this);
}

void ResourceTexture::FreeMemory()
{
	glDeleteTextures(1, &id);
	width = 0;
	height = 0;
	id = 0;
}

bool ResourceTexture::ReadBaseInfo(const char* assets_path)
{
	bool ret = true;

	name = App->file_system->GetBaseFileName(assets_path);
	this->path = assets_path;
	std::string alien_path = App->file_system->GetPathWithoutExtension(path) + "_meta.alien";

	name = App->file_system->GetBaseFileName(assets_path);

	JSON_Value* value = json_parse_file(alien_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(alien_path.data(), object, value);
		ID = std::stoull(meta->GetString("Meta.ID"));
		ReadTextureValues(meta);
		delete meta;
	}

	meta_data_path = LIBRARY_TEXTURES_FOLDER + std::to_string(ID) + ".dds";

	if (!App->file_system->Exists(meta_data_path.data())) {
		return false;
	}

	struct stat fileMeta;
	struct stat fileAssets;

	if (stat(meta_data_path.c_str(), &fileMeta) == 0 && stat(path.c_str(), &fileAssets) == 0) {
		if (fileAssets.st_mtime > fileMeta.st_mtime) {
			remove(meta_data_path.data());
			App->file_system->Copy(path.data(), meta_data_path.data());
		}
	}
	//for library
	string new_meta = LIBRARY_TEXTURES_FOLDER + std::to_string(ID) + ".meta";

	if (!App->file_system->Exists(new_meta.data())) {
		return false;
	}

	struct stat fileMeta2;
	struct stat fileAssets2;

	if (stat(new_meta.c_str(), &fileMeta2) == 0 && stat(alien_path.c_str(), &fileAssets2) == 0) {
		if (fileAssets2.st_mtime > fileMeta2.st_mtime) {
			remove(new_meta.data());
			App->file_system->Copy(alien_path.data(), new_meta.data());
		}
	}

	App->resources->AddResource(this);

	return ret;
}

void ResourceTexture::ReadLibrary(const char* meta_data)
{
	this->meta_data_path = meta_data;
	ID = std::stoull(App->file_system->GetBaseFileName(meta_data_path.data()));

	string meta_d = App->file_system->GetPathWithoutExtension(meta_data_path) + ".meta";

	JSON_Value* value = json_parse_file(meta_d.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_d.data(), object, value);
		ReadTextureValues(meta);
		delete meta;
	}
	App->resources->AddResource(this);

}

bool ResourceTexture::DeleteMetaData()
{
	remove(meta_data_path.data());

	std::vector<Resource*>::iterator position = std::find(App->resources->resources.begin(), App->resources->resources.end(), static_cast<Resource*>(this));
	if (position != App->resources->resources.end())
		App->resources->resources.erase(position);

	delete this;

	return true;
}

void ResourceTexture::SaveTextureValues(JSONfilepack* file)
{
	file->SetNumber("Meta.Wrap", wrap_type);
	file->SetNumber("Meta.Filter", texture_filter);
}

void ResourceTexture::ReadTextureValues(JSONfilepack* file)
{
	try {
		wrap_type = (int)file->GetNumber("Meta.Wrap");
		texture_filter = (int)file->GetNumber("Meta.Filter");
	}
	catch (...) {
		wrap_type = 0;
		texture_filter = 0;
	}
}

void ResourceTexture::DisplayTextureOnInspector()
{
	if (ImGui::CollapsingHeader(GetName(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Combo("Texture parametres", &wrap_type, "ClampToEdge\0Repeat\0MirroredRepeat\0ClampToBorder\0");
		ImGui::Combo("Texture filter", &texture_filter, "Nearest\0Arroundir\0NearestMipMapNearest\0LinearMipMapNearest\0NearestMipMapLinear\0LinearMipMapLinear\0");
		ImGui::Text("References : %i", references);

		if(ImGui::Button("Save"))
		{
			if (ilLoadImage(path.data())) {
				if (ID == 0)
					ID = App->resources->GetRandomID();

				std::string alien_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien").data();

				JSON_Value* alien_value = json_value_init_object();
				JSON_Object* alien_object = json_value_get_object(alien_value);
				json_serialize_to_file_pretty(alien_value, alien_path.data());

				if (alien_value != nullptr && alien_object != nullptr) {
					JSONfilepack* alien = new JSONfilepack(alien_path.data(), alien_object, alien_value);
					alien->StartSave();
					alien->SetString("Meta.ID", std::to_string(ID).data());
					SaveTextureValues(alien);
					alien->FinishSave();
					delete alien;
				}

				std::string new_meta = std::string(LIBRARY_TEXTURES_FOLDER + std::to_string(ID) + ".meta");
				std::string ext;
				App->file_system->SplitFilePath(new_meta.data(), nullptr, nullptr, &ext);
				if (App->StringCmp(ext.data(), "meta")) {
					App->file_system->Copy(alien_path.data(), new_meta.data());
				}
			}
		}
	}

}
	
void ResourceTexture::IncreaseReferences()
{
	if (ignore_next_increase) {
		ignore_next_increase = false;
	}
	else {
		if (references == 0) {
			LoadMemory();
		}
		if (App->objects->enable_instancies) {
			++references;
		}
	}
}


