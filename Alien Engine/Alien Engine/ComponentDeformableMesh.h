#pragma once
#include "ComponentMesh.h"
#include <vector>

class ResourceMesh;
class ComponentBone;
class math::float4x4;

class __declspec(dllexport) ComponentDeformableMesh : public ComponentMesh
{
	friend class ResourceModel;
	friend class GameObject;
	friend class ReturnZ;
	friend class ResourceModel;
	friend class ResourcePrefab;
public:
	ComponentDeformableMesh(GameObject* attach);
	virtual ~ComponentDeformableMesh();

	void AttachSkeleton(ComponentTransform* root);
	void AttachSkeleton();

protected:
	void AttachBone(ComponentTransform* bone_transform);
	void UpdateBonesMatrix();

	void DrawScene() override; 
	void DrawGame() override; 

	void DrawPolygon() override;
	void SetUniforms(ResourceMaterial* material) override;
	
	void SetShadowUniforms(ResourceMaterial* resource_material, ComponentCamera* camera, const float4x4& ViewMat, const float4x4& ProjMatrix, const float3& position) override;
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	void SendWeightsAndID();
	void FillWeights(int boneID, ComponentBone* Component_Bone);

private:
	std::vector<ComponentBone*> bones;
	GameObject* root_bone = nullptr;
	u64 rootID = 0;

	math::float4x4* bones_matrix = nullptr;
	ComponentMaterial* material = nullptr;	
};
