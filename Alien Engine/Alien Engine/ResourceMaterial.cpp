#include "ResourceMaterial.h"
#include "Application.h"

ResourceMaterial::ResourceMaterial() : Resource()
{
	type = ResourceType::RESOURCE_MATERIAL;
}

ResourceMaterial::~ResourceMaterial()
{
}

bool ResourceMaterial::CreateMetaData(const u64& force_id)
{
	if (force_id == 0) {
		ID = App->resources->GetRandomID();
	}
	else {
		ID = force_id;
	}

	meta_data_path = LIBRARY_MATERIALS_FOLDER + std::to_string(ID) + ".alienMaterial";

	JSON_Value* alien_value = json_value_init_object();
	JSON_Object* alien_object = json_value_get_object(alien_value);
	json_serialize_to_file_pretty(alien_value, meta_data_path.data());

	if (alien_value != nullptr && alien_object != nullptr)
	{
		JSONfilepack* alien = new JSONfilepack(meta_data_path, alien_object, alien_value);
		alien->StartSave();

		alien->SetColor("Material.Color", color);

		alien->FinishSave();
		delete alien;
	}

	return true;
}

bool ResourceMaterial::ReadBaseInfo(const char* assets_file_path)
{
	ReadLibrary(assets_file_path);
	return true;
}

void ResourceMaterial::ReadLibrary(const char* meta_data)
{
	meta_data_path = meta_data;

	JSON_Value* value = json_parse_file(meta_data_path.data());
	JSON_Object* object = json_value_get_object(value);

	ID = std::stoull(App->file_system->GetBaseFileName(meta_data));

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_data_path, object, value);

		color = meta->GetColor("Material.Color");

		delete meta;
	}
}

bool ResourceMaterial::DeleteMetaData()
{
	remove(meta_data_path.data());
	return true;
}
