#pragma once

#include "Component.h"

class __declspec(dllexport) ComponentCurve : public Component {
public:

	ComponentCurve(GameObject* attach);
	virtual ~ComponentCurve();

private:
	bool DrawInspector();

	void Clone(Component* clone);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

public:

};

