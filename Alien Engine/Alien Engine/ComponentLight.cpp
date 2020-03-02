#include "ComponentLight.h"
#include "glew/include/glew.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ReturnZ.h"
#include "ComponentMesh.h"
#include "Gizmos.h"
#include "mmgr/mmgr.h"

ComponentLight::ComponentLight(GameObject* attach) : Component(attach)
{
	type = ComponentType::LIGHT;
	light_type = LightType::UNKNOWN;

	CheckLightType(this->light_type);

#ifndef GAME_VERSION
	bulb = new ComponentMesh(game_object_attached);
	bulb->mesh = App->resources->light_mesh;
#endif
}

ComponentLight::ComponentLight(GameObject* attach, const LightType& light_type) : Component(attach)
{
	this->type = ComponentType::LIGHT;
	this->light_type = light_type;

	CheckLightType(this->light_type);

#ifndef GAME_VERSION
	bulb = new ComponentMesh(game_object_attached);
	bulb->mesh = App->resources->light_mesh;
#endif
}

ComponentLight::~ComponentLight()
{
#ifndef GAME_VERSION
	delete bulb;
#endif
}

void ComponentLight::LightLogic()
{
	ComponentTransform* transform=(ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	float pos[] = { transform->GetGlobalPosition().x, transform->GetGlobalPosition().y, transform->GetGlobalPosition().z, 1.F };
}

void ComponentLight::CheckLightType(const LightType& light_type)
{
	switch (light_type)
	{
	case LightType::POINT:
		item_current = "Point";
		break;
	case LightType::SPOT:
		item_current = "Spot";
		break;
	case LightType::DIRECTIONAL:
		item_current = "Directional";
		break;
	case LightType::AREA:
		item_current = "Area";
		break;
	case LightType::UNKNOWN:
		item_current = "NONE";
		break;
	default:
		break;
	}
}

bool ComponentLight::DrawInspector()
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

	if (ImGui::CollapsingHeader("Light", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Light");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushID("printiconlight");
		ImGui::Checkbox("Print Icon", &print_icon);
		ImGui::PopID();
	
		if (ImGui::BeginCombo("Light Type", item_current))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (item_current == items[n]);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current = items[n];
					if (item_current == "Point")
						light_type = LightType::POINT;
					else if (item_current == "Spot")
						light_type = LightType::SPOT;
					else if (item_current == "Directional")
						light_type = LightType::DIRECTIONAL;
					else if (item_current == "Area")
						light_type = LightType::AREA;
					else if (item_current == "None")
						light_type = LightType::UNKNOWN;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			// TODO: SAVE LOAD

			ImGui::EndCombo();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else
		RightClickMenu("Light");

	return true;
}

void ComponentLight::OnDisable()
{

}

void ComponentLight::Clone(Component* clone)
{
	clone->enabled = enabled;
	clone->not_destroy = not_destroy;
	ComponentLight* light = (ComponentLight*)clone;
	light->renderer_id = renderer_id;
	light->print_icon = print_icon;
}

void ComponentLight::Reset()
{
	print_icon = true;
}

void ComponentLight::SetComponent(Component* component)
{
	if (component->GetType() == type) {

		ComponentLight* light = (ComponentLight*)component;

		renderer_id = light->renderer_id;
		print_icon = light->print_icon;
	}
}

void ComponentLight::SetLightType(const LightType& type)
{
	light_type = type;
}

LightType ComponentLight::GetLightType() const
{
	return light_type;
}

void ComponentLight::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetBoolean("PrintIcon", print_icon);
}

void ComponentLight::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	ID = std::stoull(to_load->GetString("ID"));
	print_icon = to_load->GetBoolean("PrintIcon");
}

void ComponentLight::DrawIconLight()
{
	if (bulb != nullptr && print_icon)
	{
		ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
		float3 pos = transform->GetGlobalPosition();
		float4x4 matrix = float4x4::FromTRS({ pos.x - 0.133f, pos.y, pos.z }, transform->GetGlobalRotation(), { 0.2f, 0.18f, 0.2f });
		glDisable(GL_LIGHTING);
		Gizmos::DrawPoly(bulb->mesh, matrix, Color(0.0f, 255.0f, 0.0f));
		glEnable(GL_LIGHTING);
	}
}