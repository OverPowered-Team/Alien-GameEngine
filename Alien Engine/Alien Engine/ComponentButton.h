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
	bool OnClicked();
	bool OnRelease();

	Color hover = { 0.0f,0.0f,0.0f,1.0f };
	Color clicked = { 0.0f,0.0f,0.0f,1.0f };
	Color pressed = { 0.0f,0.0f,0.0f,1.0f };
	Color disabled = { 0.0f,0.0f,0.0f,1.0f };
};


#endif // !_COMPONENT_BUTTON_H
