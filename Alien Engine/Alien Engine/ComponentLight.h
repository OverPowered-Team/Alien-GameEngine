#pragma once

#include "Component.h"
#include "Color.h"
#include "ComponentMesh.h"

enum class LightType
{
	POINT,
	SPOT,
	DIRECTIONAL,
	AREA,

	UNKNOWN = -1
};

class __declspec(dllexport) ComponentLight : public Component {
	friend class GameObject;
	friend class ComponentMesh;
public:
	ComponentLight(GameObject* attach);
	ComponentLight(GameObject* attach, const LightType& light_type);
	virtual ~ComponentLight();

private:
	void LightLogic();
	void CheckLightType(const LightType& light_type);

	bool DrawInspector();
	void OnDisable();

	void Clone(Component* clone);

	void Reset();
	void SetComponent(Component* component);

	void SetLightType(const LightType& type);
	LightType GetLightType() const;

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void DrawIconLight();

private:
	ComponentMesh* bulb = nullptr;
	bool print_icon = true;

	LightType light_type = LightType::UNKNOWN;

	const char* items[5] = { "NONE", "Point", "Spot", "Directional", "Area" };
	const char* item_current = "";

	uint renderer_id = 0;
};
