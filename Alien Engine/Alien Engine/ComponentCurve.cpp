#include "ComponentCurve.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"
#include "Maths.h"
#include "glew/include/glew.h"
#include "Bezier/tinysplinecxx.h"
#include "Application.h"
#include "ModuleInput.h"

static 	tinyspline::BSpline spline(5, 3);
static tinyspline::BSpline beizer;
ComponentCurve::ComponentCurve(GameObject* attach) : Component(attach)
{
	type = ComponentType::CURVE;

	std::vector<tinyspline::real> controlPoints = spline.controlPoints();
	controlPoints[0] = -10;
	controlPoints[1] = 0;
	controlPoints[2] = 10;

	controlPoints[3] = 0;
	controlPoints[4] = 10;
	controlPoints[5] = -10;

	controlPoints[6] = 10;
	controlPoints[7] = 0;
	controlPoints[8] = 30;

	controlPoints[9] = 20;
	controlPoints[10] = 20;
	controlPoints[11] = 4;

	controlPoints[12] = 40;
	controlPoints[13] = -20;
	controlPoints[14] = -3;

	spline.setControlPoints(controlPoints);

	beizer = spline.toBeziers();
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
		
		for (uint i = 0; i < beizer.controlPoints().size(); i += 3) {
			ImGui::PushID(3213 + i);
			float3 point(beizer.controlPoints()[i], beizer.controlPoints()[i + 1], beizer.controlPoints()[i + 2]);
			if (ImGui::DragFloat3("##FFF", point.ptr())) {
				beizer.setControlPointAt(i/3, { point.x, point.y, point.z });
			}
			ImGui::PopID();
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
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		spline = tinyspline::BSpline(6,3);
		std::vector<tinyspline::real> controlPoints = spline.controlPoints();
		controlPoints[0] = -10;
		controlPoints[1] = 0;
		controlPoints[2] = 10;

		controlPoints[3] = 0;
		controlPoints[4] = 10;
		controlPoints[5] = -10;

		controlPoints[6] = 10;
		controlPoints[7] = 0;
		controlPoints[8] = 30;

		controlPoints[9] = 20;
		controlPoints[10] = 20;
		controlPoints[11] = 4;

		controlPoints[12] = 40;
		controlPoints[13] = -20;
		controlPoints[14] = -3;

		controlPoints[12] = 100;
		controlPoints[13] = 160;
		controlPoints[14] = 120;

		spline.setControlPoints(controlPoints);

		beizer = spline.toBeziers();

	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		spline = tinyspline::BSpline(4, 3);
		std::vector<tinyspline::real> controlPoints = spline.controlPoints();
		controlPoints[0] = -10;
		controlPoints[1] = 0;
		controlPoints[2] = 10;

		controlPoints[3] = 0;
		controlPoints[4] = 10;
		controlPoints[5] = -10;

		controlPoints[6] = 10;
		controlPoints[7] = 0;
		controlPoints[8] = 30;

		controlPoints[9] = 20;
		controlPoints[10] = 20;
		controlPoints[11] = 4;

		spline.setControlPoints(controlPoints);

		beizer = spline.toBeziers();
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		beizer.setControlPointAt(1, { -50, 10, 20 });
	}

	glPointSize(9);
	glBegin(GL_POINTS);
	for (uint i = 0; i < beizer.controlPoints().size(); i += 3) {
		glVertex3f(beizer.controlPoints()[i], beizer.controlPoints()[i + 1], beizer.controlPoints()[i + 2]);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (float f = 0.0F; f <= 1; f += 0.001) {
		auto res = beizer(f).result();
		glVertex3f(res[0], res[1], res[2]);
	}
	glEnd();

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
