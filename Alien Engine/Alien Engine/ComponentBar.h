#ifndef _COMPONENT_BAR_H_
#define _COMPONENT_BAR_H_

#include "ComponentUI.h"

class ResourceTexture;

class __declspec(dllexport) ComponentBar :public ComponentUI
{
public:
	ComponentBar(GameObject* obj);
	~ComponentBar() {};

	bool DrawInspector();

	void Reset() {};
	void SetComponent(Component* component) {};
	void Clone(Component* clone) {};

	void SaveComponent(JSONArraypack* to_save) {};
	void LoadComponent(JSONArraypack* to_load) {};

private:
	GameObject* bar = nullptr;

	float maxValue = 100.0f, value = 0.0f;
};

#endif // !_COMPONENT_BAR_H_
