#ifndef _COMPONENT_CHECKBOX_H_
#define _COMPONENT_CHECKBOX_H_

#include "ComponentUI.h"
#include "Color.h"
#include <functional>

class ResourceTexture;
class ComponentCanvas;

class __declspec(dllexport) ComponentCheckbox :public ComponentUI
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

	void AddListenerOnHover(std::function<void()> funct);
	void AddListenerOnClick(std::function<void()> funct);
	void AddListenerOnClickRepeat(std::function<void()> funct);
	void AddListenerOnRelease(std::function<void()> funct);

	void SetCheckboxState(bool value);

private:
	void CallListeners(std::vector<std::function<void()>>* listeners);

private:
	bool active = true;

public:
	bool clicked = false;

private:
	std::vector<std::function<void()>> listenersOnHover;
	std::vector<std::function<void()>> listenersOnClick;
	std::vector<std::function<void()>> listenersOnClickRepeat;
	std::vector<std::function<void()>> listenersOnRelease;

	ComponentCanvas* GetCanvas();
	GameObject* tick = nullptr;
	GameObject* cross = nullptr;
};









#endif // !_COMPONENT_CHECKBOX_H_
