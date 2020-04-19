#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class ComponentMesh;
class btShapeHull;

class __declspec(dllexport)  ComponentConvexHullCollider : public ComponentCollider
{
	friend class ModulePhysics;
	friend class GameObject;

public:

	ComponentConvexHullCollider(GameObject* go);
	void SetSize(float3 size);

private:

	void DrawSpecificInspector();

	void DrawScene();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void CreateDefaultShape();
	void UpdateShape();

	PxShape* CreateConvexMesh(const GameObject* go);

private:

	ComponentMesh* mesh = nullptr;
	float3 size = float3::zero();


};
