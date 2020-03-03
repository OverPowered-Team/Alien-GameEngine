#pragma once

#include "Component.h"
#include "Color.h"
#include "ComponentMesh.h"
#include "ModuleObjects.h"

struct SpotLightProperties
{
	float3 position = float3::zero();
	float3 direction = float3::zero();
	float3 ambient = float3::zero();
	float3 diffuse = float3::zero();
	float3 specular = float3::zero();
	float3 constant = float3::zero();
	float3 linear = float3::zero();
	float3 quadratic = float3::zero();
	float3 cut_off = float3::zero();
	float3 outer_cut_off = float3::zero();
};

class __declspec(dllexport) ComponentLightSpot : public Component {
	friend class GameObject;
	friend class ComponentMesh;
public:
	ComponentLightSpot(GameObject* attach);
	virtual ~ComponentLightSpot();

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

	SpotLightProperties light_props;
};
