#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "ResourceTexture.h"

ResourceMesh::ResourceMesh() : Resource()
{
	type = ResourceType::RESOURCE_MESH;
}

ResourceMesh::~ResourceMesh()
{	
	FreeMemory();
}

bool ResourceMesh::CreateMetaData(const u64& force_id)
{
	if (parent_name.empty()) {
		parent_name.assign("null");
	}
	if (force_id == 0) {
		ID = App->resources->GetRandomID();
	}
	else {
		ID = force_id;
	}

	meta_data_path = std::string(LIBRARY_MESHES_FOLDER + std::to_string(ID) + ".alienMesh");

	uint ranges[9] = { num_index, num_vertex, num_faces, family_number, (normals != nullptr) ? true : false, 
		(uv_cords != nullptr) ? true : false,  (texture != nullptr) ? true : false, parent_name.size(), name.size() };

	uint texture_size = (texture != nullptr) ? sizeof(u64) : 0;
	uint uv_size = (uv_cords != nullptr) ? sizeof(float) * num_vertex * 3 : 0;
	uint normals_size = (normals != nullptr) ? sizeof(float) * num_vertex * 3 + sizeof(float) * num_faces * 3 +
		sizeof(float) * num_faces * 3 : 0;
	uint vertex_size = sizeof(float) * num_vertex * 3;
	uint index_size = sizeof(uint) * num_index;

	uint size = sizeof(ranges) + parent_name.size() + name.size() + texture_size +
		sizeof(float) * 4 + sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 3 + vertex_size +
		index_size + normals_size + uv_size;

	char* data = new char[size]; 
	char* cursor = data;

	uint bytes = sizeof(ranges); 
	memcpy(cursor, ranges, bytes);
	cursor += bytes; 

	bytes = parent_name.size();
	memcpy(cursor, parent_name.c_str(), bytes);
	cursor += bytes;

	bytes = name.size();
	memcpy(cursor, name.c_str(), bytes);
	cursor += bytes;

	if (texture != nullptr) {
		bytes = sizeof(u64);
		memcpy(cursor, &texture->GetID(), bytes);
		cursor += bytes;
	}

	bytes = sizeof(float) * 4;
	memcpy(cursor, &material_color, bytes);
	cursor += bytes;

	bytes = sizeof(float) * 3;
	memcpy(cursor, pos.ptr(), bytes);
	cursor += bytes;

	bytes = sizeof(float) * 3;
	memcpy(cursor, rot.ptr(), bytes);
	cursor += bytes;

	bytes = sizeof(float) * 3;
	memcpy(cursor, scale.ptr(), bytes);
	cursor += bytes;

	bytes = sizeof(float) * num_vertex * 3;
	memcpy(cursor, vertex, bytes);
	cursor += bytes;

	bytes = sizeof(uint) * num_index;
	memcpy(cursor, index, bytes);
	cursor += bytes;

	if (normals != nullptr) {
		bytes = sizeof(float) * num_vertex * 3;
		memcpy(cursor, normals, bytes);
		cursor += bytes;

		bytes = sizeof(float) * num_faces * 3;
		memcpy(cursor, center_point, bytes);
		cursor += bytes;

		bytes = sizeof(float) * num_faces * 3;
		memcpy(cursor, center_point_normal, bytes);
		cursor += bytes;
	}

	if (uv_cords != nullptr) {
		bytes = sizeof(float) * num_vertex * 3;
		memcpy(cursor, uv_cords, bytes);
	}

	App->file_system->Save(meta_data_path.data(), data, size);

	delete[] data;

	return true;
}

bool ResourceMesh::ReadBaseInfo(const char* meta_file_path)
{
	bool ret = true;

	meta_data_path = std::string(meta_file_path);
	ID = std::stoull(App->file_system->GetBaseFileName(meta_file_path));

	char* data = nullptr;
	uint size = App->file_system->Load(meta_data_path.data(), &data);

	if (size > 0) {

	}
	else {
		return false;
	}

	JSON_Value* value = json_parse_file(meta_data_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_data_path, object, value);
		// names
		parent_name = meta->GetString("Mesh.ParentName");
		name = meta->GetString("Mesh.Name");
		family_number = meta->GetNumber("Mesh.FamilyNumber");
		// texture path
		bool has_texture = meta->GetBoolean("Mesh.HasTexture");
		if (has_texture) {
			texture_id = std::stoull(meta->GetString("Mesh.Texture"));
		}
		material_color = meta->GetColor("Mesh.MatColor");
		// transformations
		// pos
		pos = meta->GetFloat3("Mesh.Position");
		// scale
		scale = meta->GetFloat3("Mesh.Scale");
		// rot
		rot = meta->GetQuat("Mesh.Rotation");
		delete meta;
		App->resources->AddResource(this);
	}
	else {
		ret = false;
	}

	return ret;
}

void ResourceMesh::FreeMemory()
{
	if (id_vertex != 0)
		glDeleteBuffers(1, &id_vertex);
	if (id_index != 0)
		glDeleteBuffers(1, &id_index);
	if (id_normals != 0)
		glDeleteBuffers(1, &id_normals);
	if (id_uv != 0)
		glDeleteBuffers(1, &id_uv);

	if (index != nullptr) {
		delete[] index;
		index = nullptr;
	}
	if (vertex != nullptr) {
		delete[] vertex;
		vertex = nullptr;
	}
	if (normals != nullptr) {
		delete[] normals;
		normals = nullptr;
	}
	if (uv_cords != nullptr) {
		delete[] uv_cords;
		uv_cords = nullptr;
	}
	if (center_point_normal != nullptr) {
		delete[] center_point_normal;
		center_point_normal = nullptr;
	}
	if (center_point != nullptr) {
		delete[] center_point;
		center_point = nullptr;
	}

	id_vertex = 0;
	id_index = 0;
	id_normals = 0;
	id_uv = 0;

	references = 0;

	num_vertex = 0;
	num_index = 0;
	num_faces = 0;
}

bool ResourceMesh::LoadMemory()
{	
	JSON_Value* value = json_parse_file(meta_data_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_data_path, object, value);

		// ranges
		num_vertex = meta->GetNumber("Mesh.NumVertex");
		num_index = meta->GetNumber("Mesh.NumIndex");
		num_faces = meta->GetNumber("Mesh.NumFaces");

		// vertex
		vertex = meta->GetNumberArray("Mesh.Vertex");

		// index
		index = meta->GetUintArray("Mesh.Index");

		// normals
		bool has_normals = meta->GetBoolean("Mesh.HasNormals");

		if (has_normals) {
			// normals
			normals = meta->GetNumberArray("Mesh.Normals");

			// center point
			center_point = meta->GetNumberArray("Mesh.CenterPoint");

			// center point normal
			center_point_normal = meta->GetNumberArray("Mesh.CenterPointNormals");
		}

		// uv
		bool has_uv = meta->GetBoolean("Mesh.HasUV");

		if (has_uv) {
			uv_cords = meta->GetNumberArray("Mesh.UV");
		}

		if (texture_id != 0) {
			texture = (ResourceTexture*)App->resources->GetResourceWithID(texture_id);
		}

		if (num_vertex != 0)
			InitBuffers();

		delete meta;
	}
	else {
		LOG_ENGINE("Error loading %s", meta_data_path.data());
	}

	return true;
}

bool ResourceMesh::DeleteMetaData()
{
	remove(meta_data_path.data());

	std::vector<Resource*>::iterator position = std::find(App->resources->resources.begin(), App->resources->resources.end(), static_cast<Resource*>(this));
	if (position != App->resources->resources.end())
		App->resources->resources.erase(position);

	delete this;

	return true;
}

void ResourceMesh::ConvertToGameObject(std::vector<std::pair<u64, GameObject*>>* objects_created)
{
	// look if is loaded
	IncreaseReferences();

	// get the parent
	GameObject* obj = nullptr;

	// if parent name is not null, search the parent name
	if (!App->StringCmp(parent_name.data(), "null") && objects_created != nullptr) {
		std::vector<std::pair<u64, GameObject*>>::iterator item = objects_created->begin();
		for (; item != objects_created->end(); ++item) {
			if ((*item).second != nullptr && App->StringCmp((*item).second->GetName(), parent_name.data()) && family_number -1 == (*item).first) {
				obj = new GameObject((*item).second);
				break;
			}
		}
		objects_created->push_back({ family_number, obj });
	}
	else if (objects_created != nullptr) { // parent name == null so, the parent must be the one created before
		obj = new GameObject(objects_created->at(0).second);
		objects_created->push_back({ family_number, obj });
	}
	else { // if objects created == nullptr then parent must be the root
		obj = new GameObject(App->objects->GetRoot(false));
	}

	obj->SetName(name.data());

	obj->AddComponent(new ComponentTransform(obj, pos, rot, scale));

	if (num_vertex != 0) {
		ComponentMesh* mesh = new ComponentMesh(obj);

		mesh->mesh = this;
		mesh->RecalculateAABB_OBB();

		obj->AddComponent(mesh);

		ComponentMaterial* material = new ComponentMaterial(obj);

		if (texture != nullptr) {
			material->SetTexture(texture);
		}
		material->color = material_color;

		obj->AddComponent(material);
	}
}

void ResourceMesh::InitBuffers()
{
	glGenBuffers(1, &id_vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) *num_vertex * 3,
		vertex, GL_STATIC_DRAW);

	// index
	glGenBuffers(1, &id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_index,
		index, GL_STATIC_DRAW);

	if (uv_cords != nullptr) {
		// UV
		glGenBuffers(1, &id_uv);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_uv);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * num_vertex * 3,
			uv_cords, GL_STATIC_DRAW);
	}

	if (normals != nullptr) {
		// normals
		glGenBuffers(1, &id_normals);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_normals);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * num_vertex * 3,
			normals, GL_STATIC_DRAW);
	}
}
