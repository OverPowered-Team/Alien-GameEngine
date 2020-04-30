#include "ComponentCurve.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"

#include "glew/include/glew.h"

static float p0[3] = { 10,10, 0 };
static float p1[3] = { 10, 50, 0 };
static float p2[3] = { 50, 50, 0 };
static float p3[3] = { 50,10, 0 };

ComponentCurve::ComponentCurve(GameObject* attach) : Component(attach)
{
	type = ComponentType::CURVE;

	std::vector<Bezier::Point> points;
	points.push_back({ 0,0 });
	points.push_back({ 10,0 });
	points.push_back({ 0,10 });
	points.push_back({ 10,10 });

	curve.SetN(3, points);
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
	glPointSize(9);
	glBegin(GL_POINTS);
	for (uint i = 0; i < curve.size(); ++i) {
		glVertex3f(curve[i].x, curve[i].y, 0);
	}
	glEnd();


	glBegin(GL_LINE_STRIP);
	for (float t = 0; t < 1; t += 0.001)
	{
		Bezier::Point p = curve.valueAt(t);
		glVertex3f(p[0], p[1], 0);
	}
	glEnd();
}