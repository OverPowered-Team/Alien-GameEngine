#include "ComponentCurve.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"

#include "glew/include/glew.h"

static int p0[2] = { 100,100 };
static int p1[2] = { 100, 500 };
static int p2[2] = { 500, 500 };
static int p3[2] = { 500,100 };

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

		ImGui::DragInt2("P0", p0);
		ImGui::DragInt2("P1", p1);
		ImGui::DragInt2("P2", p2);
		ImGui::DragInt2("P3", p3);

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
	glPointSize(1);
	glColor3f(1, 1, 0);
	glBegin(GL_POINTS);
	for (float t = 0; t < 1; t += 0.001)
	{
		int* p = bezier(t, p0, p1, p2, p3);
		glVertex3f(p[0], p[1], 0);
	}
	glEnd();

	glPointSize(9);
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	glVertex3f(p0[0], p0[1], 0);
	glColor3f(0, 1, 0);
	glVertex3f(p1[0], p1[1], 0);
	glColor3f(0, 0, 1);
	glVertex3f(p2[0], p2[1], 0);
	glColor3f(1, 1, 1);
	glVertex3f(p3[0], p3[1], 0);
	glEnd();

	glPointSize(1);
}

int* ComponentCurve::bezier(float t, int* p0, int* p1, int* p2, int* p3)
{
	int res[2];
	res[0] = pow((1 - t), 3) * p0[0] + 3 * t * pow((1 - t), 2) * p1[0] + 3 * pow(t, 2) * (1 - t) * p2[0] + pow(t, 3) * p3[0];
	res[1] = pow((1 - t), 3) * p0[1] + 3 * t * pow((1 - t), 2) * p1[1] + 3 * pow(t, 2) * (1 - t) * p2[1] + pow(t, 3) * p3[1];
	return res;
}