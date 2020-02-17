#pragma once

#include "C_Collider.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class C_Mesh;

class ComponentSphereCollider : public ComponentCollider
{
public:

	friend ModulePhysics;

public:

	ComponentSphereCollider(GameObject* go);

	// Virtual Functions ------------------------------

	void CreateShape(C_Mesh* mesh);

	void AdjustShape();

	void SaveCollider(Config& config);

	void LoadCollider(Config& config);

	void DrawPanelColliderInfo();

private:

	float radius;
};
