#include "Resource_.h"
#include "ModuleObjects.h"
#include "Application.h"

Resource::Resource()
{
}

Resource::~Resource()
{
}

const char* const Resource::GetAssetsPath() const
{
	return path.data();
}

const char* const Resource::GetLibraryPath() const
{
	return meta_data_path.data();
}

void Resource::SetAssetsPath(const char* path)
{
	this->path = std::string(path);
}

void Resource::SetLibraryPath(const char* path)
{
	this->meta_data_path = std::string(path);
}

const char* const Resource::GetName() const
{
	return name.data();
}

void Resource::SetName(const char* name)
{
	this->name = std::string(name);
}

const ResourceType Resource::GetType() const
{
	return type;
}

const u64& Resource::GetID() const
{
	return ID;
}

const bool Resource::NeedToLoad() const
{
	return references == 0;
}

void Resource::IncreaseReferences()
{
	if (references == 0) {
		if (!LoadMemory())
		{
			LOG_ENGINE("Couldn't load from memory %s", this->name.c_str());
		}
	}
	if (App->objects->enable_instancies) {
		++references;
	}
}

void Resource::DecreaseReferences()
{
	if (App->objects->enable_instancies && references > 0) {
		--references;
	}
	if (references == 0) {
		FreeMemory();
	}
}
