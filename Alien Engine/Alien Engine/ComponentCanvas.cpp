#include "ComponentCanvas.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "glew/include/glew.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"


ComponentCanvas::ComponentCanvas(GameObject* obj, uint w, uint h):Component(obj)
{
	width = 160;
	height = 90;

	type = ComponentType::CANVAS;
}

bool ComponentCanvas::DrawInspector()
{
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Canvas", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Canvas");



		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Canvas");
	}

	return true;
}

void ComponentCanvas::Draw()
{
	ComponentTransform* comp_trans = game_object_attached->GetComponent<ComponentTransform>();

	glBegin(GL_LINE_LOOP);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	float3 pos = comp_trans->GetGlobalPosition();

	float3 v1 = float3(pos.x, pos.y, pos.z);
	float3 v2 = float3(pos.x + width, pos.y, pos.z);
	float3 v3 = float3(pos.x + width, pos.y - height, pos.z);
	float3 v4 = float3(pos.x, pos.y - height, pos.z);

	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
	glVertex3f(v4.x, v4.y, v4.z);

	glEnd();
}
