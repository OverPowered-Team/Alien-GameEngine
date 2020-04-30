#pragma once

#include "Component.h"

class __declspec(dllexport) ComponentCurve : public Component {
public:

	ComponentCurve(GameObject* attach);
	virtual ~ComponentCurve();

private:

	void DrawScene();

	void print(int x, int y, char* string);

	int* bezier(float t, int* p0, int* p1, int* p2, int* p3);

	bool DrawInspector();

	void Clone(Component* clone);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

public:

};

