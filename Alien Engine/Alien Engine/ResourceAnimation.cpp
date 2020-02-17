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
	return false;
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
