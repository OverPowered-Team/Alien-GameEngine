#ifndef _COMPONENT_BUTTON_H_
#define _COMPONENT_BUTTON_H_

#include "ComponentUI.h"
#include "Color.h"

class ResourceTexture;

class ComponentButton :public ComponentUI
{
public:
	ComponentButton(GameObject* obj);
	~ComponentButton() {};

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
};


#endif // !_COMPONENT_BUTTON_H
