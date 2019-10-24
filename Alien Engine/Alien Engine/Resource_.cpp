#include "Resource_.h"

Resource::Resource()
{
}

Resource::~Resource()
{
}

const char* const Resource::GetPath() const
{
	return path;
}

const char* const Resource::GetName() const
{
	return name.data();
}

const ResourceType Resource::GetType() const
{
	return type;
}
