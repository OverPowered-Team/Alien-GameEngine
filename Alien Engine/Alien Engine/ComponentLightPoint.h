#pragma once

#include "Component.h"
#include "Color.h"
#include "ComponentMesh.h"
#include "ModuleObjects.h"

struct PointLightProperties
{
	float3 position = float3::zero();
	float3 direction = float3::zero();
	float3 ambient = float3::zero();
	float3 diffuse = float3::zero();
	float3 specular = float3::zero();
	float3 constant = float3::zero();
	float3 linear = float3::zero();
	float3 quadratic = float3::zero();
};

class __declspec(dllexport) ComponentLightPoint : public Component {
	friend class GameObject;
	friend class ComponentMesh;
public:
	ComponentLightPoint(GameObject* attach);
	virtual ~ComponentLightPoint();

private:
	void LightLogic();

	bool DrawInspector();
	void OnDisable();

	void Clone(Component* clone);

	void Reset();
	void SetComponent(Component* component);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void DrawIconLight();

private:
	ComponentMesh* bulb = nullptr;
	bool print_icon = true;

	uint renderer_id = 0;

	PointLightProperties light_props;
};
