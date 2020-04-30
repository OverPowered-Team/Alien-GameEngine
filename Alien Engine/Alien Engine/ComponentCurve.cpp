#include "ComponentCurve.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"
#include "Maths.h"
#include "glew/include/glew.h"

static float p0[3] = { 10,10, 0 };
static float p1[3] = { 10, 50, 0 };
static float p2[3] = { 50, 50, 0 };
static float p3[3] = { 50,10, 0 };

ComponentCurve::ComponentCurve(GameObject* attach) : Component(attach)
{
	type = ComponentType::CURVE;
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

		ImGui::DragFloat3("P0", p0);
		ImGui::DragFloat3("P1", p1);
		ImGui::DragFloat3("P2", p2);
		ImGui::DragFloat3("P3", p3);

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
	//glPointSize(1);
	//glColor3f(1, 1, 0);
	//glBegin(GL_LINE_STRIP);
	//for (float t = 0; t < 1; t += 0.001)
	//{
	//	float* p = bezier(t, p0, p1, p2, p3);
	//	glVertex3f(p[0], p[1], p[2]);
	//}
	//glEnd();

	//glPointSize(9);
	//glBegin(GL_POINTS);
	//glColor3f(1, 0, 0);
	//glVertex3f(p0[0], p0[1], p0[2]);
	//glColor3f(0, 1, 0);
	//glVertex3f(p1[0], p1[1], p1[2]);
	//glColor3f(0, 0, 1);
	//glVertex3f(p2[0], p2[1], p2[2]);
	//glColor3f(1, 1, 1);
	//glVertex3f(p3[0], p3[1], p3[2]);
	//glEnd();

	//glPointSize(1);


	std::vector<float3> points;

	points.push_back({ 10,0,0 });
	points.push_back({ -10,0,0 });
	points.push_back({ 0,0,0 });

	Curve curve;
	curve.CreatePoints(0.001, points);
	
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

float* ComponentCurve::bezier(float t, float* p0, float* p1, float* p2, float* p3)
{
	float res[3];
	res[0] = pow((1 - t), 3) * p0[0] + 3 * t * pow((1 - t), 2) * p1[0] + 3 * pow(t, 2) * (1 - t) * p2[0] + pow(t, 3) * p3[0];
	res[1] = pow((1 - t), 3) * p0[1] + 3 * t * pow((1 - t), 2) * p1[1] + 3 * pow(t, 2) * (1 - t) * p2[1] + pow(t, 3) * p3[1];
	res[2] = pow((1 - t), 3) * p0[2] + 3 * t * pow((1 - t), 2) * p1[2] + 3 * pow(t, 2) * (1 - t) * p2[2] + pow(t, 3) * p3[2];
	return res;
}

uint Curve::Size()
{
	return control_points.size();
}

float3 Curve::ValueAt(float at)
{
	if (at < 0 || at > 1 || curve_points.empty() || control_points.empty()) {
		return float3::zero();
	}

	uint index = Maths::Map(at, 0, 1, 0, curve_points.size() - 1);
	return curve_points[index];
}

static float3 center(float3 p1, float3 p2) {

	return float3(

		(p1.x + p2.x) / 2,

		(p1.y + p2.y) / 2,

		(p1.z + p2.z) / 2

	);
}

static double quadBezierPoint(double a0, double a1, double a2, double t) {

	return Pow(1 - t, 2) * a0 + 2 * (1 - t) * t * a1 + Pow(t, 2) * a2;

}


static float3 quadBezier(float3 p1, float3 p2, float3 p3, double t) {

	return float3(

		quadBezierPoint(p1.x, p2.x, p3.x, t),

		quadBezierPoint(p1.y, p2.y, p3.y, t),

		quadBezierPoint(p1.z, p2.z, p3.z, t));

}

static double cubicBezierPoint(double a0, double a1, double a2, double a3, double t) {

	return Pow(1 - t, 3) * a0 + 3 * Pow(1 - t, 2) * t * a1 + 3 * (1 - t) * Pow(t, 2) * a2 + Pow(t, 3) * a3;

}


static float3 cubicBezier(float3 p1, float3 p2, float3 p3, float3 p4, double t) {

	return float3(

		cubicBezierPoint(p1.x, p2.x, p3.x, p4.x, t),

		cubicBezierPoint(p1.y, p2.y, p3.y, p4.y, t),

		cubicBezierPoint(p1.z, p2.z, p3.z, p4.z, t));

}

void Curve::CreatePoints(float detail, const std::vector<float3>& controlPoints)
{
	float3 res = float3::zero();

	//generate the end and control points
	for (int i = 1; i < controlPoints.size() - 1; i += 2) {

		control_points.push_back(center(controlPoints[i - 1], controlPoints[i]));
		control_points.push_back(controlPoints[i]);
		control_points.push_back(controlPoints[i + 1]);

		if (i + 2 < controlPoints.size() - 1) {
			control_points.push_back(center(controlPoints[i + 1], controlPoints[i + 2]));
		}
	}

	//Generate the detailed points. 
	float3 a0, a1, a2, a3;

	for (int i = 0; i < control_points.size() - 2; i += 4) {

		a0 = control_points[i];
		a1 = control_points[i + 1];
		a2 = control_points[i + 2];

		if (i + 3 > control_points.size() - 1) {
			//quad
			for (double j = 0; j < 1; j += detail) {
				curve_points.push_back(quadBezier(a0, a1, a2, j));
			}

		}
		else {
			//cubic
			a3 = control_points[i + 3];

			for (double j = 0; j < 1; j += detail) {
				curve_points.push_back(cubicBezier(a0, a1, a2, a3, j));
			}
		}
	}
	//Tuple3d[] points = new Tuple3d[renderingPoints.size()];

	//renderingPoints.toArray(points);

	//return res;
}

const std::vector<float3>& Curve::GetControlPoints()
{
	return control_points;
}
