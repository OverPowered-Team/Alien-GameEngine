#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class C_Mesh;

class ComponentSphereCollider : public ComponentCollider
{
public:

	friend class GameObject;
	friend class ModulePhysics;

public:

	ComponentSphereCollider(GameObject* go);
	void SetRadius(float radius);
	float GetRadius() { return radius; }

private:

	void DrawSpecificInspector();;
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void CreateDefaultShape();

private:

	float radius = 0.5f;
};
