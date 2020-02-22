#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;

class ComponentBoxCollider : public ComponentCollider
{
	friend ModulePhysics;

public:

	ComponentBoxCollider(GameObject* go);

private:

	float3 CheckInvalidCollider(float3 size);

	// Virtual Functions ------------------------------

	void CreateShape(ComponentMesh* mesh);

	void AdjustShape();

	void Reset();

	void SaveComponent(JSONArraypack* to_save);

	void LoadComponent(JSONArraypack* to_load);

	bool DrawInspector();



private:

	float3 size;

};

