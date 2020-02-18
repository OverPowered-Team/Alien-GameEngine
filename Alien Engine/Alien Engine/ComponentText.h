#ifndef _COMPONENT_TEXT_H_
#define _COMPONENT_TEXT_H_

#include "ComponentUI.h"

class ResourceFont;

class ComponentText :public ComponentUI
{
public:
	ComponentText(GameObject* obj);
	virtual ~ComponentText() {};

	bool DrawInspector();
	bool DrawCharacter();

	void Draw(bool isGame) override;
	void Reset() {};
	void SetComponent(Component* component) {};
	void Clone(Component* clone) {};

	void SaveComponent(JSONArraypack* to_save) {};
	void LoadComponent(JSONArraypack* to_load) {};

private:
	std::string		text;
	ResourceFont*	font;
};

#endif // !_COMPONENT_TEXT_H_
