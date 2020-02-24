#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;

class ComponentBoxCollider : public ComponentCollider
{
	friend class ModulePhysics;
	friend class GameObject;

public:

	ComponentBoxCollider(GameObject* go);

private:

	float3 CheckInvalidCollider(float3 size);

	// Virtual Functions ------------------------------

	bool DrawInspector();

	void Reset();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void CreateShape();
	void AdjustShapeToMesh();
	void AdjustToScale();

private:

	float3 size;

};

