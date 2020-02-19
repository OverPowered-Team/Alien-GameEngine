#ifndef _COMPONENT_SLIDER_H_
#define _COMPONENT_SLIDER_H_

#include "ComponentUI.h"

class GameObject;
class ResourceTexture;


class __declspec(dllexport) ComponentSlider : public ComponentUI
{
public:
	ComponentSlider(GameObject* obj);
	~ComponentSlider() {};

	bool DrawInspector();

	void Draw(bool isGame) override;
	void Update() override;

	void DrawTexture(bool isGame, ResourceTexture* tex);

	void Reset();
	void SetComponent(Component* component);
	void Clone(Component* clone);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	bool OnHover();
	bool OnClick();
	bool OnPressed();
	bool OnRelease();
private:
	void UILogic();
	bool CheckMouseInside(float3 mouse_pos);

public:

	ResourceTexture* sliderTexture = nullptr;
	float sliderScaleX = 0.2F;
	float sliderScaleY = 0.9F;
};

#endif // !_COMPONENT_SLIDER_H_
