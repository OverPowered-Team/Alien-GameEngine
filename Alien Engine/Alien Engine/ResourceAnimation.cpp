#include "Application.h"
#include "ModuleImporter.h"
#include "ResourceAnimation.h"


ResourceAnimation::ResourceAnimation() : Resource()
{
}

ResourceAnimation::~ResourceAnimation()
{
	
}


std::string ResourceAnimation::GetTypeString() const
{
	return "Animation";
}

float ResourceAnimation::GetDuration() const
{
	return ((float)tick_duration / (float)ticks_per_second);
}

uint ResourceAnimation::GetChannelIndex(std::string name)
{
	uint i;
	for (i = 0; i < num_channels; i++) {
		if (channels[i].name == name) {
			return i;
		}
	}

	return num_channels;
}


void ResourceAnimation::Load()
{
	//App->importer->animation->Load(this);
}

bool ResourceAnimation::CreateMetaData(const u64& force_id)
{
	if (force_id == 0) {
		ID = App->resources->GetRandomID();
	}
	else {
		ID = force_id;
	}
	
	meta_data_path = std::string(LIBRARY_MESHES_FOLDER + std::to_string(ID) + ".alienMesh");

	uint size = sizeof(uint) + name.size() + sizeof(uint) * 6 + sizeof(bool);
	for (uint i = 0; i < num_channels; i++)
	{
		size += sizeof(uint) +  channels[i].name.size() + sizeof(uint) * 3 +
			sizeof(KeyAnimation<float3>) *  channels[i].num_position_keys + sizeof(KeyAnimation<float3>) *  channels[i].num_scale_keys + sizeof(KeyAnimation<Quat>) *  channels[i].num_rotation_keys;
	}

	// Allocate
	char* data = new char[size];
	char* cursor = data;

	// Store name size and name
	uint bytes = sizeof(uint);
	uint name_size =  name.size();
	memcpy(cursor, &name_size, bytes);
	cursor += bytes;
	bytes =  name.size();
	memcpy(cursor,  name.c_str(), bytes);
	cursor += bytes;

	// Store duration, ticks and num channels
	bytes = sizeof(uint);
	memcpy(cursor, & start_tick, bytes);
	cursor += bytes;
	memcpy(cursor, & end_tick, bytes);
	cursor += bytes;
	memcpy(cursor, & tick_duration, bytes);
	cursor += bytes;
	memcpy(cursor, & ticks_per_second, bytes);
	cursor += bytes;
	memcpy(cursor, & max_tick, bytes);
	cursor += bytes;
	memcpy(cursor, & num_channels, bytes);
	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(cursor, & loops, bytes);
	cursor += bytes;

	// Store channels
	for (uint i = 0; i <  num_channels; i++)
	{
		//Store channel name size and name
		bytes = sizeof(uint);
		name_size =  channels[i].name.size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes =  channels[i].name.size();
		memcpy(cursor,  channels[i].name.c_str(), bytes);
		cursor += bytes;

		//Store num position, scale, rotation keys
		bytes = sizeof(uint);
		memcpy(cursor, & channels[i].num_position_keys, bytes);
		cursor += bytes;
		memcpy(cursor, & channels[i].num_scale_keys, bytes);
		cursor += bytes;
		memcpy(cursor, & channels[i].num_rotation_keys, bytes);
		cursor += bytes;

		//Store position keys
		bytes = sizeof(KeyAnimation<float3>) *  channels[i].num_position_keys;
		memcpy(cursor,  channels[i].position_keys, bytes);
		cursor += bytes;

		//Store scale keys
		bytes = sizeof(KeyAnimation<float3>) *  channels[i].num_scale_keys;
		memcpy(cursor,  channels[i].scale_keys, bytes);
		cursor += bytes;

		//Store rotation keys
		bytes = sizeof(KeyAnimation<Quat>) *  channels[i].num_rotation_keys;
		memcpy(cursor,  channels[i].rotation_keys, bytes);
		cursor += bytes;
	}
	App->file_system->Save(meta_data_path.data(), data, size);
	RELEASE_ARRAY(data);
	return true;
}

bool ResourceAnimation::DeleteMetaData()
{
	if (channels == nullptr)
		return;

	LOG_ENGINE("Unloading Animation %s", name.c_str());

	for (int i = 0; i < num_channels; i++)
	{
		delete[] channels[i].position_keys;
		delete[] channels[i].scale_keys;
		delete[] channels[i].rotation_keys;
	}

	channels = nullptr;
}
