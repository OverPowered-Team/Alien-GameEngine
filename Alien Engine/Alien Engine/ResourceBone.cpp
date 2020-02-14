#include "Application.h"
#include "ModuleImporter.h"

#include "ResourceBone.h"



ResourceBone::ResourceBone() : Resource()
{
}


ResourceBone::~ResourceBone()
{
}

std::string ResourceBone::GetTypeString() const
{
	return "Bone";
}

void ResourceBone::Load()
{
	//App->importer->bone->Load(this);
}

void ResourceBone::UnLoad()
{
	delete[] vertex_ids;
	delete[] weights;
}
