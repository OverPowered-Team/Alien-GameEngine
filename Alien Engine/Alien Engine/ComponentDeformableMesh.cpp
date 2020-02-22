#include "Component.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ResourceMesh.h"
#include "ComponentDeformableMesh.h"

ComponentDeformableMesh::ComponentDeformableMesh(GameObject* attach) : ComponentMesh(attach)
{
	type = ComponentType::DEFORMABLE_MESH;
}

ComponentDeformableMesh::~ComponentDeformableMesh()
{
	/*if (game_object_attached != nullptr && game_object_attached->HasComponent(ComponentType::MATERIAL))
	{
		static_cast<ComponentMaterial*>(game_object_attached->GetComponent(ComponentType::MATERIAL))->not_destroy = false;
	}
	if (mesh != nullptr && mesh->is_custom) {
		mesh->DecreaseReferences();
	}*/
	//clear deformable mesh?
}

void ComponentDeformableMesh::AttachSkeleton(ComponentTransform* root)
{
	//Duplicate mesh
	//if (mesh)
		//deformable_mesh = new ResourceMesh(App->GenerateID(), mesh);
	AttachBone(root);
}

void ComponentDeformableMesh::AttachBone(ComponentTransform* bone_transform)
{
	ComponentBone* c_bone = (ComponentBone*)bone_transform->game_object_attached->GetComponent(ComponentType::BONE);

	if (c_bone)
		bones.push_back(c_bone);

	for each (GameObject* go in bone_transform->game_object_attached->GetChildren())
		AttachBone(go->transform);
}
