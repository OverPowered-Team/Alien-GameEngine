#include "ComponentCurve.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"
#include "Maths.h"
#include "glew/include/glew.h"
#include "Application.h"
#include "ModuleInput.h"

ComponentCurve::ComponentCurve(GameObject* attach) : Component(attach)
{
	type = ComponentType::CURVE;

	CreateBaseCurve();
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

void ComponentCurve::CreateBaseCurve()
{
	beizer = tinyspline::BSpline(4, 3, 3, tsBSplineType::TS_BEZIERS);

	std::vector<tinyspline::real> control_points = beizer.controlPoints();

	control_points[0] = -10;
	control_points[1] = 0;
	control_points[2] = 0;

	control_points[3] = -5;
	control_points[4] = 0;
	control_points[5] = 0;

	control_points[6] = 0;
	control_points[7] = 0;
	control_points[8] = 0;

	control_points[9] = 5;
	control_points[10] = 0;
	control_points[11] = 0;

	beizer.setControlPoints(control_points);
}

void ComponentCurve::DrawScene()
{	
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
