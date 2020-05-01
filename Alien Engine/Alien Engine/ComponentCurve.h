#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"
#include <vector>

class __declspec(dllexport) Curve {
public:
	Curve() {}
	Curve(const float3& begin, const float3& end);
	~Curve() {}

	// 0 - 1
	float3 ValueAt(float at);

	const std::vector<float3>& GetControlPoints();

	void SetControlPointAt(int index, const float3& value);

	void SetDetail(int detail);

	void AddSegment(bool begin);

private:

	void Refresh();

	void CalculateCurvePoints();
	float3 CalculateBezier(float t, const float3& p0, const float3& p1, const float3& p2, const float3& p3);

public:

	int detail = 50;

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