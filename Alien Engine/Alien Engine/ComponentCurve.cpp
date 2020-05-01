#include "ComponentCurve.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"
#include "Maths.h"
#include "glew/include/glew.h"
#include "imgui/imgui_internal.h"

#include "Application.h"
#include "ModuleInput.h"

ComponentCurve::ComponentCurve(GameObject* attach) : Component(attach)
{
	type = ComponentType::CURVE;

	curve = Curve(float3{ -10,0,0 }, float3{ 10,0,0 });
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
		ImGui::Spacing();
		if (ImGui::DragInt("Detail", &curve.detail)) {
			curve.SetDetail(curve.detail);
		}
		ImGui::Spacing();

		if (ImGui::Button("Add Segment in the beggining")) {
			curve.AddSegment(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Add Segment in the end")) {
			curve.AddSegment(false);
		}
		
		ImGui::Spacing();
		if (ImGui::TreeNodeEx("Segments", ImGuiTreeNodeFlags_SpanAvailWidth)) {
			for (uint i = 0; i < curve.GetControlPoints().size() - 1; i += 3) {
				if (ImGui::TreeNodeEx(std::string("Segment " + std::to_string(int(i / (float)3) + 1)).data(), ImGuiTreeNodeFlags_SpanAvailWidth)) {
					ImGui::Spacing();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Start Point");
					ImGui::SameLine(140);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					ImGui::PushID(323123 + i);
					if (ImGui::DragFloat3("##FF", (float*)curve.GetControlPoints()[i].ptr())) {
						curve.SetControlPointAt(i, curve.GetControlPoints()[i]);
					}
					ImGui::PopID();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("End Point");
					ImGui::SameLine(140);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					ImGui::PushID(323123 + i + 3);
					if (ImGui::DragFloat3("##FF", (float*)curve.GetControlPoints()[i + 3].ptr())) {
						curve.SetControlPointAt(i + 3, curve.GetControlPoints()[i + 3]);
					}
					ImGui::PopID();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Tensor 1");
					ImGui::SameLine(140);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					ImGui::PushID(323123 + i + 1);
					if (ImGui::DragFloat3("##FF", (float*)curve.GetControlPoints()[i + 1].ptr())) {
						curve.SetControlPointAt(i + 1, curve.GetControlPoints()[i + 1]);
					}
					ImGui::PopID();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Tensor 2");
					ImGui::SameLine(140);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					ImGui::PushID(323123 + i + 2);
					if (ImGui::DragFloat3("##FF", (float*)curve.GetControlPoints()[i + 2].ptr())) {
						curve.SetControlPointAt(i + 2, curve.GetControlPoints()[i + 2]);
					}
					ImGui::PopID();
					float width = ImGui::GetItemRectSize().x - 8;
					ImGui::Spacing();
					

					if (curve.GetControlPoints().size() == 4) {
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					}

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 91);
					ImGui::PushID(989 + i);
					if (ImGui::Button("Remove Begin", { width * 0.5F, 0 })) {
						curve.RemoveControlPoint(i);
						ImGui::PopID();
						ImGui::TreePop();
						break;
					}
					ImGui::PopID();
					ImGui::SameLine();
					ImGui::PushID(64343 + i);
					if (ImGui::Button("Remove End", { width * 0.5F, 0 })) {
						curve.RemoveControlPoint(i + 3);
						ImGui::PopID();
						ImGui::TreePop();
						break;
					}
					ImGui::PopID();


					if (curve.GetControlPoints().size() == 4) {
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
					ImGui::Spacing();
					ImGui::PushID(32 + i);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 91);
					if (ImGui::Button("Add Control Point", { width + 8, 0 })) {
						curve.InsertControlPoint(i);
					}
					ImGui::PopID();


					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
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
	glDisable(GL_LIGHTING);
	glPointSize(9);
	glBegin(GL_POINTS);
	for (uint i = 0; i < curve.GetControlPoints().size(); ++i) {
		if (i % 3 == 0) {
			glColor3f(0, 1, 0);
		}
		else {
			glColor3f(1, 0, 0);
		}
		glVertex3f(curve.GetControlPoints()[i].x, curve.GetControlPoints()[i].y, curve.GetControlPoints()[i].z);
	}
	glEnd();

	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int f = 0; f <= curve.detail; ++f) {
		float3 res = curve.ValueAt(f/(float)curve.detail);
		glVertex3f(res[0], res[1], res[2]);
	}
	glEnd();

	glEnable(GL_LIGHTING);
}

Curve::Curve(const float3& begin, const float3& end)
{
	control_points.push_back(begin);
	control_points.push_back(begin + float3(5,10,0));
	control_points.push_back(end + float3(-5, 10, 0));
	control_points.push_back(end);

	CalculateCurvePoints();
}

float3 Curve::ValueAt(float at)
{
	if (at < 0 || at > 1 || curve_points.empty() || control_points.empty()) {
		return float3::zero();
	}

	uint index = Maths::Map(at, 0, 1, 0, curve_points.size() - 1);
	return curve_points[index];
}

const std::vector<float3>& Curve::GetControlPoints()
{
	return control_points;
}

void Curve::SetControlPointAt(int index, const float3& value)
{
	control_points[index] = value;
	Refresh();
}

void Curve::SetDetail(int detail)
{
	this->detail = detail;
	Refresh();
}

void Curve::AddSegment(bool begin)
{
	float3 newPoint = float3::zero();
	float3 tensor1 = float3::zero();
	float3 tensor2 = float3::zero();

	if (begin) {
		newPoint = control_points.front() + float3(-10, 0, 0);
		tensor1 = newPoint + float3(2.5F, 10, 0);
		tensor2 = control_points.front() + float3(-2.5F, 0, 0);
		
		control_points.insert(control_points.begin(), tensor2);
		control_points.insert(control_points.begin(), tensor1);
		control_points.insert(control_points.begin(), newPoint);
	}
	else {
		newPoint = control_points.back() + float3(10, 0, 0);
		tensor1 = control_points.back() + float3(2.5F, 10, 0);
		tensor2 = newPoint + float3(-2.5F, 10, 0);

		control_points.push_back(tensor1);
		control_points.push_back(tensor2);
		control_points.push_back(newPoint);
	}

	Refresh();
}

void Curve::InsertControlPoint(int index)
{
	float3 begin = control_points[index];
	float3 end = control_points[index + 3];

	float3 mid = (begin + end) * 0.5F;
	float3 tensor1 = mid + float3(-5, 10, 0);
	float3 tensor2 = mid + float3(5, 10, 0);

	std::vector<float3> points;
	points.push_back(tensor1);
	points.push_back(mid);
	points.push_back(tensor2);

	control_points.insert(control_points.begin() + index + 2, points.begin(), points.end());

	Refresh();
}

void Curve::RemoveControlPoint(int index)
{
	if (index == 0) {
		control_points.erase(control_points.begin());
		control_points.erase(control_points.begin());
		control_points.erase(control_points.begin());
	}
	else if (index == control_points.size() - 1) {
		control_points.pop_back();
		control_points.pop_back();
		control_points.pop_back();
	}
	else {
		control_points.erase(control_points.begin() + --index);
		control_points.erase(control_points.begin() + index);
		control_points.erase(control_points.begin() + index);
	}

	Refresh();
}

void Curve::Refresh()
{
	curve_points.clear();

	CalculateCurvePoints();
}

void Curve::CalculateCurvePoints()
{
	for (uint i = 0; i < control_points.size() - 1; i += 3) {
		for (int f = 0; f <= detail; ++f) {
			curve_points.push_back(CalculateBezier(f/(float)detail, control_points[i], control_points[i + 1], control_points[i + 2], control_points[i + 3]));
		}
	}
}

float3 Curve::CalculateBezier(float t, const float3& p0, const float3& p1, const float3& p2, const float3& p3)
{
	float3 res = float3::zero();
	res[0] = pow((1 - t), 3) * p0[0] + 3 * t * pow((1 - t), 2) * p1[0] + 3 * pow(t, 2) * (1 - t) * p2[0] + pow(t, 3) * p3[0];
	res[1] = pow((1 - t), 3) * p0[1] + 3 * t * pow((1 - t), 2) * p1[1] + 3 * pow(t, 2) * (1 - t) * p2[1] + pow(t, 3) * p3[1];
	res[2] = pow((1 - t), 3) * p0[2] + 3 * t * pow((1 - t), 2) * p1[2] + 3 * pow(t, 2) * (1 - t) * p2[2] + pow(t, 3) * p3[2];
	return res;
}