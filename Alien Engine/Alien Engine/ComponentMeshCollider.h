#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"

class GameObject;

class __declspec(dllexport) ComponentMeshCollider : public ComponentCollider
{
	friend class GameObject;

public:
	ComponentMeshCollider(GameObject* go);
	void SetSize(const float3 size);
	const float3 GetSize() const { return size; }

private:
	void ScaleChanged();
	void DrawSpecificInspector();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	void Clone(Component* clone);
	void Reset();

	PxShape* CreateMeshShape();

private:
	PxTriangleMesh* triangle_mesh = nullptr;
	float3 size = float3::one();
};

