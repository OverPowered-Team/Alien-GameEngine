#ifndef _COMPONENT_CHECKBOX_H_
#define _COMPONENT_CHECKBOX_H_

#include "ComponentUI.h"
#include "Color.h"

class ResourceTexture;
class ComponentCanvas;

class ComponentCheckbox :public ComponentUI
{
public:

	ComponentCheckbox(GameObject* obj);
	~ComponentCheckbox() {};

	bool DrawInspector();

	bool OnHover();
	bool OnClick();
	bool OnPressed();
	bool OnRelease();

	Color idle_color = { 0.8f,0.8f,0.8f,1.0f };
	Color hover_color = { 1.0f,1.0f,1.0f,1.0f };
	Color clicked_color = { 0.7f,0.7f,0.7f,1.0f };
	Color pressed_color = { 0.75f,0.75f,0.75f,1.0f };
	Color disabled_color = { 0.3f,0.3f,0.3f,1.0f };

	void SetActive(bool active);

private:
	bool active = true;
	bool clicked = false;

private:
	ComponentCanvas* GetCanvas();
	GameObject* tick = nullptr;
	GameObject* cross = nullptr;
};









#endif // !_COMPONENT_CHECKBOX_H_
