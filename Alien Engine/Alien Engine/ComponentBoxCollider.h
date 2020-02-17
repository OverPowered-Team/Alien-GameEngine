#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class C_Mesh;

class ComponentBoxCollider : public ComponentCollider
{
public:

	friend ModulePhysics;

public:

	ComponentBoxCollider(GameObject* go);

	// Virtual Functions ------------------------------

	void CreateShape(C_Mesh* mesh);

	void AdjustShape();

	void SaveCollider(Config& config);

	void LoadCollider(Config& config);

	void DrawPanelColliderInfo();

private:

	float3 CheckInvalidCollider(float3 size);


private:

	float3 size;

};

