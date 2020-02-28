#pragma once
#include "ComponentMesh.h"
#include <vector>

class ResourceMesh;
class ComponentBone;

class ComponentDeformableMesh : public ComponentMesh
{
	friend class GameObject;
	friend class ReturnZ;
	friend class ResourceModel;
public:
	ComponentDeformableMesh(GameObject* attach);
	virtual ~ComponentDeformableMesh();

	void AttachSkeleton(ComponentTransform* root);
	void AttachSkeleton();

protected:
	void AttachBone(ComponentTransform* bone_transform);
	void UpdateDeformableMesh();

	void DrawPolygon();
	void DrawOutLine();
	void DrawMesh();
	
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	void SendWeightsAndID();
	void FillWeights(int boneID, ComponentBone* Component_Bone);
private:
	ResourceMesh* deformable_mesh = nullptr;
	std::vector<ComponentBone*> bones;
	u64 root_bone_id = 0;
	float* weights = nullptr;
	int* bones_ID = nullptr;

};
