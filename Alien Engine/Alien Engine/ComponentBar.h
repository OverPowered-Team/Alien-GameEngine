#ifndef _COMPONENT_BAR_H_
#define _COMPONENT_BAR_H_

#include "ComponentUI.h"

class ResourceTexture;

class __declspec(dllexport) ComponentBar :public ComponentUI
{
public:
	ComponentBar(GameObject* obj);
	~ComponentBar() {};

	void Draw(bool isGame);
	void DrawTexture(bool isGame, ResourceTexture* tex);
	bool DrawInspector();

	void Reset() {};
	void SetComponent(Component* component) {};
	void Clone(Component* clone) {};

	void SaveComponent(JSONArraypack* to_save) {};
	void LoadComponent(JSONArraypack* to_load) {};

private:
	ResourceTexture* barTexture = nullptr;
	float barScaleX = 0.2F;
	float barScaleY = 0.9F;

	int maxValue = 0, value = 100;
};

#endif // !_COMPONENT_BAR_H_
