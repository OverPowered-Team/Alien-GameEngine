#include "Application.h"
#include "ModuleImporter.h"
#include "ResourceAnimation.h"


ResourceAnimation::ResourceAnimation(uint id) : Resource()
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

void ResourceAnimation::UnLoad()
{
	LOG_ENGINE("Unloading Animation %s", name.c_str());

	for (int i = 0; i < num_channels; i++)
	{
		delete[] channels[i].position_keys;
		delete[] channels[i].scale_keys;
		delete[] channels[i].rotation_keys;
	}

	channels = nullptr;
}

void ResourceAnimation::Load()
{
	//App->importer->animation->Load(this);
}