#ifndef _COMPONENT_IMAGE_H_
#define _COMPONENT_IMAGE_H_

#include "ComponentUI.h"

class ResourceFont;

class ComponentText :public ComponentUI
{
public:
	ComponentText(GameObject* obj);
	~ComponentText() {};

	bool DrawInspector();

	void Reset() {};
	void SetComponent(Component* component) {};
	void Clone(Component* clone) {};

	void SaveComponent(JSONArraypack* to_save) {};
	void LoadComponent(JSONArraypack* to_load) {};
};

#endif // !_COMPONENT_IMAGE_H_
