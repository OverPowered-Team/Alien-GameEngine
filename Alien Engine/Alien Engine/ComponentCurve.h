#pragma once

#include "Component.h"
#include "Bezier/bezier.h"

class __declspec(dllexport) ComponentCurve : public Component {
public:

	ComponentCurve(GameObject* attach);
	virtual ~ComponentCurve();

private:

	void DrawScene();


	float* bezier(float t, float* p0, float* p1, float* p2, float* p3);

	bool DrawInspector();

	void Clone(Component* clone);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

public:

};

