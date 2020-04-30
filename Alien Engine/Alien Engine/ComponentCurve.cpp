#include "ComponentCurve.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"
#include "Maths.h"
#include "glew/include/glew.h"

ComponentCurve::ComponentCurve(GameObject* attach) : Component(attach)
{
	type = ComponentType::CURVE;

	curve.CreatePoints(0.001, { -10,0,0 }, { -10,10,0 }, { 10,10,0 }, { 10,0,0 });
}

ComponentCurve::~ComponentCurve()
{
}

bool ComponentCurve::DrawInspector()
{
	static bool en;
	ImGui::PushID(this);
	en = enabled;
	if (ImGui::Checkbox("##CmpActive", &en)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = en;
		if (!enabled)
			OnDisable();
		else
			OnEnable();
	}

	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Curve", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Curve");
		bool reset = false;
		for (uint i = 0; i < curve.GetControlPoints().size(); ++i) {
			ImGui::PushID(&curve.GetControlPoints()[i]);
			if (ImGui::DragFloat3("##FFFF", (float*)curve.GetControlPoints()[i].ptr())) {
				reset = true;
			}
			ImGui::PopID();
		}
		if (reset) {
			std::vector<float3> points;
			points.assign(curve.GetControlPoints().begin(), curve.GetControlPoints().end());
			curve.CreatePoints(0.001F, points[0], points[1], points[2], points[3]);
		}

	}
	else {
		RightClickMenu("Curve");
	}

	return true;
}

void ComponentCurve::Clone(Component* clone)
{
}

void ComponentCurve::SaveComponent(JSONArraypack* to_save)
{
}

void ComponentCurve::LoadComponent(JSONArraypack* to_load)
{
}

void ComponentCurve::DrawScene()
{	
	glBegin(GL_LINE_STRIP);
	for (float f = 0; f <= 1; f += 0.001F) {
		float3 p = curve.ValueAt(f);
		glVertex3f(p[0], p[1], p[2]);
	}
	glEnd();

	glPointSize(9);
	glBegin(GL_POINTS);
	for (uint i = 0; i < curve.GetControlPoints().size(); ++i) {
		float3 p = curve.GetControlPoints()[i];
		glVertex3f(p[0], p[1], p[2]);
	}
	glEnd();
	glPointSize(1);
}

float3 Curve::ValueAt(float at)
{
	if (at < 0 || at > 1 || curve_points.empty() || control_points.empty()) {
		return float3::zero();
	}

	uint index = Maths::Map(at, 0, 1, 0, curve_points.size() - 1);
	return curve_points[index];
}

void Curve::CreatePoints(float detail, const float3& p0, const float3& p1, const float3& p2, const float3& p3)
{
	float3 res = float3::zero();

	control_points.clear();
	curve_points.clear();

	control_points.push_back(p0);
	control_points.push_back(p1);
	control_points.push_back(p2);
	control_points.push_back(p3);

	for (float t = 0; t < 1; t += detail) {
		curve_points.push_back(bezier(t, (float*)p0.ptr(), (float*)p1.ptr(), (float*)p2.ptr(), (float*)p3.ptr()));
	}
}

const std::vector<float3>& Curve::GetControlPoints()
{
	return control_points;
}

float3 Curve::bezier(float t, float* p0, float* p1, float* p2, float* p3)
{
	float3 res = float3::zero();
	res[0] = pow((1 - t), 3) * p0[0] + 3 * t * pow((1 - t), 2) * p1[0] + 3 * pow(t, 2) * (1 - t) * p2[0] + pow(t, 3) * p3[0];
	res[1] = pow((1 - t), 3) * p0[1] + 3 * t * pow((1 - t), 2) * p1[1] + 3 * pow(t, 2) * (1 - t) * p2[1] + pow(t, 3) * p3[1];
	res[2] = pow((1 - t), 3) * p0[2] + 3 * t * pow((1 - t), 2) * p1[2] + 3 * pow(t, 2) * (1 - t) * p2[2] + pow(t, 3) * p3[2];
	return res;
}
