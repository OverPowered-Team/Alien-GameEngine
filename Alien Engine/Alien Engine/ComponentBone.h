#ifndef _COMPONENT_BONE_H_
#define _COMPONENT_BONE_H_

#include "Component.h"

class ResourceBone;
class ComponentTransform;

class ComponentBone :
	public Component
{
public:
	ComponentBone(GameObject* gameobject);
	~ComponentBone();
public:
	bool debug_draw = false;
	void Draw();
	void AddBone(ResourceBone* b);
	uint GetBoneID();
	ResourceBone* GetBone();

private:
	ResourceBone* bone = nullptr;
};

#endif // !_COMPONENT_BONE_H_



