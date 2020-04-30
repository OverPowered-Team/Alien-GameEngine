#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"
#include <vector>

class __declspec(dllexport) Curve {
public:
	Curve() {}
	~Curve() {}

	uint Size();

	// 0 - 1
	float3 ValueAt(float at);

	void CreatePoints(float detail, const std::vector<float3>& controlPoints);

	const std::vector<float3>& GetControlPoints();

private:

	std::vector<float3> control_points;
	std::vector<float3> curve_points;

};

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

