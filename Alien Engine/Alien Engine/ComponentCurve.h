#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"
#include <vector>

class __declspec(dllexport) Curve {
public:
	Curve() {}
	~Curve() {}

	// 0 - 1
	float3 ValueAt(float at);

	void CreatePoints(float detail, const float3& p0, const float3& p1, const float3& p2, const float3& p3);

	const std::vector<float3>& GetControlPoints();

private:

	float3 bezier(float t, float* p0, float* p1, float* p2, float* p3);

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

	bool DrawInspector();

	void Clone(Component* clone);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

public:

	Curve curve;

};

