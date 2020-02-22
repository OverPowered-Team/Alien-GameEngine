#include "ResourceModel.h"
#include "Globals.h"
#include "ResourceMesh.h"
#include "ResourceAnimation.h"
#include "ModuleFileSystem.h"
#include "Application.h"
#include <algorithm>
#include "ReturnZ.h"
#include "ComponentTransform.h"

ResourceModel::ResourceModel() : Resource()
{
	type = ResourceType::RESOURCE_MODEL;
}

ResourceModel::~ResourceModel()
{
	std::vector<ResourceMesh*>::iterator item = meshes_attached.begin();
	for (; item != meshes_attached.end(); ++item) {
		if (*item != nullptr) { // aixo passa pq es fa el delete del modul resources i aquest respurce mesh ja la borrat alla
			delete* item;
			*item = nullptr;
		}
	}
	meshes_attached.clear();
}

bool ResourceModel::CreateMetaData(const u64& force_id)
{	
	if (force_id == 0)
		ID = App->resources->GetRandomID();
	else
		ID = force_id;

	std::string* meta_mesh_paths = nullptr;
	std::string* meta_animation_paths = nullptr;
	int num_anims = 0;
	if (force_id != 0) 
	{
		JSON_Value* value = json_parse_file(meta_data_path.data());
		JSON_Object* object = json_value_get_object(value);

		if (value != nullptr && object != nullptr)
		{
			JSONfilepack* meta = new JSONfilepack(meta_data_path.data(), object, value);

			meta_mesh_paths = meta->GetArrayString("Meta.PathMeshes");
			meta_animation_paths = meta->GetArrayString("Meta.PathAnimations");
			num_anims = meta->GetNumber("Meta.NumAnimations");
			delete meta;
		}
	}

	std::string alien_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");

	JSON_Value* alien_value = json_value_init_object();
	JSON_Object* alien_object = json_value_get_object(alien_value);
	json_serialize_to_file_pretty(alien_value, alien_path.data());

	if (alien_value != nullptr && alien_object != nullptr) 
	{
		JSONfilepack* alien = new JSONfilepack(alien_path, alien_object, alien_value);
		alien->StartSave();
		alien->SetString("Meta.ID", std::to_string(ID));

		std::string library_path = std::string(LIBRARY_MODELS_FOLDER) + std::string(std::to_string(ID) + ".alienModel");

		JSON_Value* model_value = json_value_init_object();
		JSON_Object* model_object = json_value_get_object(model_value);
		json_serialize_to_file_pretty(model_value, library_path.data());

		if (model_value != nullptr && model_object != nullptr) 
		{
			JSONfilepack* meta = new JSONfilepack(library_path, model_object, model_value);

			meta->StartSave();
			meta->SetString("Model.Name", name);
			meta->SetNumber("Model.NumMeshes", meshes_attached.size());
			alien->SetNumber("Meta.NumMeshes", meshes_attached.size());

			std::string* meshes_paths = new std::string[meshes_attached.size()];
			std::vector<ResourceMesh*>::iterator item = meshes_attached.begin();
			for (; item != meshes_attached.end(); ++item) {
				if ((*item) != nullptr) 
				{
					if (meta_mesh_paths != nullptr) {
						std::string path_ = App->file_system->GetBaseFileName(meta_mesh_paths[item - meshes_attached.begin()].data()); //std::stoull().data());
						(*item)->CreateMetaData(std::stoull(path_));
					}
					else {
						(*item)->CreateMetaData();
					}

					meshes_paths[item - meshes_attached.begin()] = (*item)->GetLibraryPath();
					LOG_ENGINE("Created alienMesh file %s", (*item)->GetLibraryPath());
				}
			}

			meta->SetNumber("Model.NumAnimations", animations_attached.size());
			alien->SetNumber("Meta.NumAnimations", animations_attached.size()); 
			JSONArraypack* anim_array = meta->InitNewArray("Model.Animations");
			JSONArraypack* anim_array_alien = alien->InitNewArray("Meta.Animations");

			std::string* animation_paths = new std::string[animations_attached.size()];
			for (int i = 0; i < animations_attached.size(); ++i)
			{
				if (meta_animation_paths != nullptr && i < num_anims)
				{
					std::string path_ = App->file_system->GetBaseFileName(meta_animation_paths[i].data()); //std::stoull().data());
					animations_attached[i]->CreateMetaData(std::stoull(path_));
				}
				else 
				{
					animations_attached[i]->CreateMetaData();
				}
				anim_array->SetAnotherNode();
				anim_array_alien->SetAnotherNode();
				anim_array->SetString("Name",animations_attached[i]->name.data());
				anim_array_alien->SetString("Name", animations_attached[i]->name.data());
				anim_array->SetBoolean("Loops", animations_attached[i]->loops);
				anim_array_alien->SetBoolean("Loops", animations_attached[i]->loops);
				anim_array->SetNumber("Start_Tick", animations_attached[i]->start_tick);
				anim_array_alien->SetNumber("Start_Tick", animations_attached[i]->start_tick);
				anim_array->SetNumber("End_Tick", animations_attached[i]->end_tick);
				anim_array_alien->SetNumber("End_Tick", animations_attached[i]->end_tick);

				animation_paths[i] = animations_attached[i]->GetLibraryPath();
				LOG_ENGINE("Created alienMesh file %s", animations_attached[i]->GetLibraryPath());
			}

			meta->SetArrayString("Model.PathMeshes", meshes_paths, meshes_attached.size());
			alien->SetArrayString("Meta.PathMeshes", meshes_paths, meshes_attached.size());
			meta->SetArrayString("Model.PathAnimations", animation_paths, animations_attached.size());
			alien->SetArrayString("Meta.PathAnimations", animation_paths, animations_attached.size());

			if (meta_mesh_paths != nullptr)
				delete[] meta_mesh_paths;
			delete[] meshes_paths;

			if (meta_animation_paths != nullptr)
				delete[] meta_animation_paths;
			delete[] animation_paths;
			// Create the file
			LOG_ENGINE("Created alien file %s", library_path.data());
			
			meta->FinishSave();
			alien->FinishSave();
			delete alien;
			delete meta;
			return true;
		}
		else {
			return false;
		}

	}
	else {
		LOG_ENGINE("Error creating meta with path %s", meta_data_path.data());
		return false;
	}
}

bool ResourceModel::ReadBaseInfo(const char* assets_file_path)
{
	bool ret = true;

	path = std::string(assets_file_path);
	meta_data_path = App->file_system->GetPathWithoutExtension(assets_file_path) + "_meta.alien";

	JSON_Value* value = json_parse_file(meta_data_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_data_path, object, value);

		ID = std::stoull(meta->GetString("Meta.ID"));
		int num_meshes = meta->GetNumber("Meta.NumMeshes");
		int num_anims = meta->GetNumber("Meta.NumAnimations");
		std::string* mesh_paths = meta->GetArrayString("Meta.PathMeshes");
		std::string* anim_paths = meta->GetArrayString("Meta.PathAnimations");

		for (uint i = 0; i < num_meshes; ++i) {
			if (!App->file_system->Exists(mesh_paths[i].data())) {
				delete[] mesh_paths;
				delete meta;
				return false;
			}
		}

		for (uint i = 0; i < num_anims; ++i) {
			if (!App->file_system->Exists(anim_paths[i].data())) {
				delete[] anim_paths;
				delete meta;
				return false;
			}
		}

		delete[] anim_paths;
		delete[] mesh_paths;
		delete meta;

		// InitMeshes
		std::string library_path = LIBRARY_MODELS_FOLDER + std::to_string(ID) + ".alienModel";
		JSON_Value* mesh_value = json_parse_file(library_path.data());
		JSON_Object* mesh_object = json_value_get_object(mesh_value);

		if (mesh_value != nullptr && mesh_object != nullptr) {

			JSONfilepack* model = new JSONfilepack(library_path, mesh_object, mesh_value);

			name = model->GetString("Model.Name");

			std::string* mesh_path = model->GetArrayString("Model.PathMeshes");
			std::string* anim_path = model->GetArrayString("Model.PathAnimations");

			for (uint i = 0; i < num_meshes; ++i) {
				ResourceMesh* r_mesh = new ResourceMesh();
				if (r_mesh->ReadBaseInfo(mesh_path[i].data())) {
					meshes_attached.push_back(r_mesh);
				}
				else {
					LOG_ENGINE("Error loading %s", mesh_path[i].data());
					delete r_mesh;
				}
			}
			for (uint i = 0; i < num_anims; ++i) {
				ResourceAnimation* r_anim = new ResourceAnimation();
				if (r_anim->ReadBaseInfo(anim_path[i].data())) {
					animations_attached.push_back(r_anim);
				}
				else {
					LOG_ENGINE("Error loading %s", anim_path[i].data());
					delete r_anim;
				}
			}
			delete[] anim_path;
			delete[] mesh_path;
			delete model;
			App->resources->AddResource(this);
		}
	}
	return ret;
}

void ResourceModel::ReadLibrary(const char* meta_data)
{
	meta_data_path = meta_data;
	ID = std::stoull(App->file_system->GetBaseFileName(meta_data_path.data()));

	JSON_Value* mesh_value = json_parse_file(meta_data_path.data());
	JSON_Object* mesh_object = json_value_get_object(mesh_value);

	if (mesh_value != nullptr && mesh_object != nullptr) {

		JSONfilepack* model = new JSONfilepack(meta_data_path, mesh_object, mesh_value);

		name = model->GetString("Model.Name");
		int num_meshes = model->GetNumber("Model.NumMeshes");
		std::string* mesh_path = model->GetArrayString("Model.PathMeshes");
		int num_anims = model->GetNumber("Model.NumAnimations");
		std::string* anim_path = model->GetArrayString("Model.PathAnimations");

		for (uint i = 0; i < num_meshes; ++i) {
			ResourceMesh* r_mesh = new ResourceMesh();
			if (r_mesh->ReadBaseInfo(mesh_path[i].data())) {
				meshes_attached.push_back(r_mesh);
			}
			else {
				LOG_ENGINE("Error loading %s", mesh_path[i].data());
				delete r_mesh;
			}
		}
		for (uint i = 0; i < num_anims; ++i) {
			ResourceAnimation* r_anim = new ResourceAnimation();
			if (r_anim->ReadBaseInfo(anim_path[i].data())) {
				animations_attached.push_back(r_anim);
			}
			else {
				LOG_ENGINE("Error loading %s", anim_path[i].data());
				delete r_anim;
			}
		}
		delete[] mesh_path;
		delete[] anim_path;
		delete model;
		App->resources->AddResource(this);
	}
}

bool ResourceModel::LoadMemory()
{
	bool ret = true;

	std::vector<ResourceMesh*>::iterator item = meshes_attached.begin();
	for (; item != meshes_attached.end(); ++item) {
		if (*item != nullptr) {
			(*item)->LoadMemory();
		}
	}

	std::vector<ResourceAnimation*>::iterator anim_item = animations_attached.begin();
	for (; anim_item != animations_attached.end(); ++anim_item) {
		if (*anim_item != nullptr) {
			(*anim_item)->LoadMemory();
		}
	}

	return ret;
}

bool ResourceModel::DeleteMetaData()
{
	remove(std::string(LIBRARY_MODELS_FOLDER + std::to_string(ID) + ".alienModel").data());

	std::vector<ResourceMesh*>::iterator item = meshes_attached.begin();
	for (; item != meshes_attached.end(); ++item) {
		if (*item != nullptr) {
			(*item)->DeleteMetaData();
		}
	}
	meshes_attached.clear();

	std::vector<ResourceAnimation*>::iterator item_anim = animations_attached.begin();
	for (; item_anim != animations_attached.end(); ++item_anim)
	{
		if ((*item_anim) != nullptr)
		{
			(*item_anim)->DeleteMetaData();
		}
	}
	animations_attached.clear();

	std::vector<Resource*>::iterator position = std::find(App->resources->resources.begin(), App->resources->resources.end(), static_cast<Resource*>(this));
	if (position != App->resources->resources.end()) 
		App->resources->resources.erase(position);

	delete this;

	return true;
}

void ResourceModel::ConvertToGameObjects()
{
	if (meshes_attached.empty())
		return;

	std::sort(meshes_attached.begin(), meshes_attached.end(), ResourceModel::SortByFamilyNumber);

	if (meshes_attached.size() > 1) { // needs an empty gameobject
		
		GameObject* parent = nullptr;
		if (meshes_attached.at(0)->family_number == meshes_attached.at(1)->family_number) {
			parent = new GameObject(App->objects->GetRoot(false));
			parent->AddComponent(new ComponentTransform(parent, { 0,0,0 }, { 0,0,0,0 }, { 1,1,1 }));
			parent->SetName(name.data());
		}
		else {
			parent = App->objects->GetRoot(false);
		}
		
		// vector to find the parents
		std::vector<std::pair<u64,GameObject*>> objects_created;
		objects_created.push_back({ 0,parent });

		std::vector<ResourceMesh*>::iterator item = meshes_attached.begin();
		for (; item != meshes_attached.end(); ++item) {
			if (*item != nullptr) {
				(*item)->ConvertToGameObject(&objects_created);
			}
		}
		objects_created.clear();

		//// set it selected
		//App->objects->SetNewSelectedObject(parent);
		//ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_OBJECT, parent);
	}
	else { 
		meshes_attached.at(0)->ConvertToGameObject(nullptr);
	}
	App->objects->SetNewSelectedObject(App->objects->GetRoot(false)->children.back());
	ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_OBJECT, App->objects->GetRoot(false)->children.back());
	App->camera->fake_camera->Look(App->objects->GetRoot(false)->children.back()->GetBB().CenterPoint());
	App->camera->reference = App->objects->GetRoot(false)->children.back()->GetBB().CenterPoint();
}

bool ResourceModel::SortByFamilyNumber(const ResourceMesh* mesh1, const ResourceMesh* mesh2)
{
	return mesh1->family_number < mesh2->family_number;
}
