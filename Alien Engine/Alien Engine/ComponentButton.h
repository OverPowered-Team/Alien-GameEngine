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

	Color idle = { 1.0f,1.0f,1.0f,0.8f };
	Color hover = { 1.0f,1.0f,1.0f,1.0f };
	Color clicked = { 1.0f,1.0f,1.0f,0.7f };
	Color pressed = { 1.0f,1.0f,1.0f,0.75f };
	Color disabled = { 1.0f,1.0f,1.0f,0.3f };
};


#endif // !_COMPONENT_BUTTON_H
