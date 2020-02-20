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

	void Reset();
	void SetComponent(Component* component);
	void Clone(Component* clone);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	bool OnHover();
	bool OnClick();
	bool OnPressed();
	bool OnRelease();

	void UILogic() override;
	bool CheckMouseInside(float3 mouse_pos) override;

private:
	GameObject* dot = nullptr;


public:

	Color idle_color = { 0.8f,0.8f,0.8f,1.0f };
	Color hover_color = { 1.0f,1.0f,1.0f,1.0f };
	Color clicked_color = { 0.7f,0.7f,0.7f,1.0f };
	Color pressed_color = { 0.75f,0.75f,0.75f,1.0f };
	Color disabled_color = { 0.3f,0.3f,0.3f,1.0f };
};

#endif // !_COMPONENT_SLIDER_H_
