#include "Application.h"
#include "Component.h"
#include "ComponentDeformableMesh.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ComponentBone.h"
#include "ComponentMaterial.h"


#include "ResourceMesh.h"
#include "ResourceBone.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"
#include <stdio.h>
#include <iostream>

ComponentDeformableMesh::ComponentDeformableMesh(GameObject* attach) : ComponentMesh(attach)
{
	type = ComponentType::DEFORMABLE_MESH;
	name = "Deformable Mesh";

	//Change this to resource model so is only one new 
	bones_matrix = new float4x4[100];
	memset(bones_matrix, 0, sizeof(float) * 100 * 4 * 4);
}

ComponentDeformableMesh::~ComponentDeformableMesh()
{
	if (game_object_attached != nullptr && game_object_attached->HasComponent(ComponentType::MATERIAL))
	{
		static_cast<ComponentMaterial*>(game_object_attached->GetComponent(ComponentType::MATERIAL))->not_destroy = false;
	}
	if (mesh != nullptr && mesh->is_custom) {
		mesh->DecreaseReferences();
	}
}

void ComponentDeformableMesh::AttachSkeleton(ComponentTransform* root)
{
	root_bone_id = root->game_object_attached->ID;
	AttachBone(root);
	
	material = (ComponentMaterial*)game_object_attached->GetComponent(ComponentType::MATERIAL);

	SendWeightsAndID();

}

void ComponentDeformableMesh::AttachSkeleton()
{
	if (root_bone_id != 0)
		AttachSkeleton(App->objects->GetGameObjectByID(root_bone_id)->transform);
}

void ComponentDeformableMesh::AttachBone(ComponentTransform* bone_transform)
{
	std::vector<ComponentBone*> c_bones = bone_transform->game_object_attached->GetComponents<ComponentBone>();

	if (c_bones.size() > 0)
	{
		for each (ComponentBone* c_bone in c_bones)
		{
			if (c_bone->GetMeshName() == mesh->name)
				bones.push_back(c_bone);
		}
	}
		

	for each (GameObject* go in bone_transform->game_object_attached->GetChildren())
		AttachBone(go->transform);
}

void ComponentDeformableMesh::UpdateBonesMatrix()
{
	uint i = 0;
	for (std::vector<ComponentBone*>::iterator it = bones.begin(); it != bones.end(); ++it, ++i)
	{
		ResourceBone* r_bone = (*it)->GetBone();
		math::float4x4 boneGlobalMatrix = (*it)->game_object_attached->transform->global_transformation;
		math::float4x4 meshMatrix = game_object_attached->transform->global_transformation.Inverted();
		math::float4x4 boneTransform = meshMatrix * boneGlobalMatrix * r_bone->matrix;
		
		bones_matrix[i] = boneTransform;
	}
}


void ComponentDeformableMesh::DrawPolygon(ComponentCamera* camera)
{
	OPTICK_EVENT();
	if (mesh == nullptr || mesh->id_index <= 0 || material == nullptr)
		return;

	UpdateBonesMatrix();

	ComponentTransform* transform = game_object_attached->transform;

	if (transform->IsScaleNegative())
		glFrontFace(GL_CW);

	// -------------------------- Actual Drawing -------------------------- 

	if (material->texture != nullptr)
		glBindTexture(GL_TEXTURE_2D, material->texture->id);

	material->used_shader->Bind();
	material->used_shader->UpdateUniforms();

	glBindVertexArray(mesh->vao);

	// Uniforms
	material->used_shader->SetUniformMat4f("view", camera->GetViewMatrix4x4()); // TODO: About in-game camera?
	material->used_shader->SetUniformMat4f("model", transform->GetGlobalMatrix().Transposed());
	material->used_shader->SetUniformMat4f("projection", camera->GetProjectionMatrix4f4());
	material->used_shader->SetUniformMat4f("gBones", bones_matrix, bones.size());

	glDrawElements(GL_TRIANGLES, mesh->num_index * 3, GL_UNSIGNED_INT, NULL);
	// --------------------------------------------------------------------- 
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	material->used_shader->Unbind();

	char boneName[250];
	math::float4x4 boneTransform = float4x4::identity();
	//for (uint i = 0; i < bones.size(); ++i)
	//{
	//	sprintf_s(boneName, "bones[%u]", i);
	//	material->used_shader->SetUniformMat4f(boneName, boneTransform);
	//}

	if (transform->IsScaleNegative())
		glFrontFace(GL_CCW);
}

void ComponentDeformableMesh::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("ViewMesh", view_mesh);
	to_save->SetBoolean("Wireframe", wireframe);
	to_save->SetBoolean("ViewVertexNormals", view_vertex_normals);
	to_save->SetBoolean("ViewFaceNormals", view_face_normals);
	to_save->SetBoolean("DrawAABB", draw_AABB);
	to_save->SetBoolean("DrawOBB", draw_OBB);
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetString("MeshID", mesh ? std::to_string(mesh->GetID()) : std::to_string(0));
	to_save->SetString("RootBoneID", root_bone_id != 0 ? std::to_string(root_bone_id) : std::to_string(0));
	to_save->SetBoolean("Enabled", enabled);
}

void ComponentDeformableMesh::LoadComponent(JSONArraypack* to_load)
{
	view_mesh = to_load->GetBoolean("ViewMesh");
	wireframe = to_load->GetBoolean("Wireframe");
	view_vertex_normals = to_load->GetBoolean("ViewVertexNormals");
	view_face_normals = to_load->GetBoolean("ViewFaceNormals");
	draw_AABB = to_load->GetBoolean("DrawAABB");
	draw_OBB = to_load->GetBoolean("DrawOBB");
	enabled = to_load->GetBoolean("Enabled");
	root_bone_id = std::stoull(to_load->GetString("RootBoneID"));
	ID = std::stoull(to_load->GetString("ID"));
	u64 mesh_ID = std::stoull(to_load->GetString("MeshID"));
	if (mesh_ID != 0)
	{
		mesh = (ResourceMesh*)App->resources->GetResourceWithID(mesh_ID);
		if (mesh != nullptr)
			mesh->IncreaseReferences();
	}

	GenerateAABB();
	RecalculateAABB_OBB();
}

//TODO change this to execute this code only one time
//When loading resouce model
void ComponentDeformableMesh::SendWeightsAndID()
{
	
	int bone_id = 0;
	//Genereting array of weights and bones_ID
	if (weights == nullptr && bones_ID == nullptr)
	{
		weights = new float[mesh->num_vertex * 4];
		bones_ID = new int[mesh->num_vertex * 4];

		memset(weights, 0, sizeof(float) * mesh->num_vertex * 4);
		memset(bones_ID, 0, sizeof(int) * mesh->num_vertex * 4);
	}

	for (std::vector<ComponentBone*>::iterator component_bone = bones.begin();
		component_bone != bones.end(); component_bone++ , bone_id++)
	{
		FillWeights(bone_id, (*component_bone));
	}
	glBindVertexArray(mesh->vao);


	if (mesh->id_bones == 0)
	{
		glGenBuffers(1, &mesh->id_bones);
	}
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_bones);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 4, bones_ID, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	if (mesh->id_weights == 0)
	{
		glGenBuffers(1, &mesh->id_weights);
	}
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_weights);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 4, weights, GL_STATIC_DRAW);

	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(4);

	//if (weights)
	//{
	//	delete weights;
	//	weights = nullptr;
	//}
	//if (bones_ID)
	//{
	//	delete bones_ID;
	//	bones_ID = nullptr;
	//}
}

void ComponentDeformableMesh::FillWeights(int bone_ID, ComponentBone* component_bone)
{
	ResourceBone* bone = component_bone->GetBone();
	for (int i = 0; i < bone->num_weights; i++)
	{
		int vertex_id = bone->vertex_ids[i];
		for (int j = vertex_id * 4; j < (vertex_id * 4) + 4; j++)
		{
			if (weights[j] == 0.0f)
			{
				weights[j] = bone->weights[i];
				bones_ID[j] = bone_ID;
				break;
			}
		}
	}
}

