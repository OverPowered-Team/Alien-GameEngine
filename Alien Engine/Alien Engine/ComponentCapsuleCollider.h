#pragma once

#include "C_Collider.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;


class ComponentCapsuleCollider : public ComponentCollider
{
public:

	friend ModulePhysics;

public:

	enum class CapsuleType
	{
		X, Y, Z
	};

	ComponentCapsuleCollider(GameObject* go);

	// Virtual Functions ------------------------------

	void CreateShape(C_Mesh* mesh);

	void AdjustShape();

	void SaveCollider(Config& config);

	void LoadCollider(Config& config);

	void DrawPanelColliderInfo();

private:

	float radius = 0.5f;
	float height = 1.f;
	CapsuleType capsule_type = CapsuleType::Y;

};
