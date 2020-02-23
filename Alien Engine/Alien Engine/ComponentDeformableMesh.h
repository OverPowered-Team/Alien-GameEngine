#pragma once
#include "ComponentMesh.h"
#include <vector>

class ResourceMesh;
class ComponentBone;

class ComponentDeformableMesh : public ComponentMesh
{
public:
	ComponentDeformableMesh(GameObject* attach);
	virtual ~ComponentDeformableMesh();

	void AttachSkeleton(ComponentTransform* root);

protected:
	void AttachBone(ComponentTransform* bone_transform);
	void UpdateDeformableMesh();
	void DrawPolygon();
	

private:
	ResourceMesh* deformable_mesh = nullptr;
	std::vector<ComponentBone*> bones;
};
