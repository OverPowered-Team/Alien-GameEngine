#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class ComponentMesh;

class ComponentSphereCollider : public ComponentCollider
{
public:

	friend class GameObject;
	friend class ModulePhysics;

public:

	ComponentSphereCollider(GameObject* go);

	/*virtual void QueryMesh(ComponentMesh* mesh) override;*/
	void SetRadius(float radius);

protected:

	void ScaleChanged();

private:

	void InitializeRadius();
	const float CalculateRadius();
	PxShape* RecreateSphereShape();

	void DrawSpecificInspector();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	void Reset();

private:

	float radius = 0.5f;
};