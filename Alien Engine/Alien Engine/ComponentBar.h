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

	void Draw(bool isGame) override;
	void Update() override;
	void DrawTexture(bool isGame, ResourceTexture* tex);

	void Reset() {};
	void SetComponent(Component* component) {};
	void Clone(Component* clone) {};

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void GetValue();

private:
	ResourceTexture* barTexture = nullptr;
	float maxValue = 100.0f, minValue = 0.0f, currentValue = 0.0f, factor = 0.0f;
	float barScaleY = 0.8F;

	float offsetX = 0.0f;


	float barX = 0.0F;
	float barY = 0.0F;
};

#endif // !_COMPONENT_BAR_H_
