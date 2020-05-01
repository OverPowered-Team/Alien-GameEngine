#pragma once

#include "Component.h"
#include <vector>
#include "Bezier/tinysplinecxx.h"

class __declspec(dllexport) ComponentCurve : public Component {
public:

	ComponentCurve(GameObject* attach);
	virtual ~ComponentCurve();

private:

	void DrawScene();

	bool DrawInspector();

	void Clone(Component* clone);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void CreateBaseCurve();

public:

	tinyspline::BSpline beizer;

};

