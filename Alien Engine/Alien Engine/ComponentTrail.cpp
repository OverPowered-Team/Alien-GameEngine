#include "Application.h"
#include "ComponentTrail.h"
#include "ModuleObjects.h"
#include "Optick/include/optick.h"
#include "Trail.h"
#include "imgui/imgui_internal.h"
#include "PanelProject.h"
#include "Time.h"
#include "ModuleResources.h"

ComponentTrail::ComponentTrail(GameObject* parent) : Component(parent)
{
	type = ComponentType::TRAIL;
	trail = new Trail(this, parent);
	
#ifndef GAME_VERSION
	//App->objects->debug_draw_list.emplace(this, std::bind(&ComponentTrail::DrawScene, this));
#endif // !GAME_VERSION
}

ComponentTrail::~ComponentTrail()
{
	delete trail;
	trail = nullptr;
}

void ComponentTrail::PreUpdate()
{
	if (trail->isPlaying())
		trail->PreUpdate(Time::GetCurrentDT());
}

void ComponentTrail::Update()
{
	if(trail->isPlaying())
		trail->Update(Time::GetCurrentDT());
}

void ComponentTrail::PostUpdate()
{
	if (trail->isPlaying())
		trail->PostUpdate(Time::GetCurrentDT());
}

void ComponentTrail::DrawScene()
{
	DebugDraw();
}

void ComponentTrail::DrawGame()
{
	OPTICK_EVENT();

#ifndef GAME_VERSION
	if (App->objects->printing_scene)
	{
		if (game_object_attached->selected)
			Draw();
	}
	else
		Draw();
#else

	Draw();

#endif
}

void ComponentTrail::DebugDraw()
{
	

}

void ComponentTrail::Draw()
{
	trail->Draw();
}

void ComponentTrail::OnEnable()
{
}

void ComponentTrail::OnDisable()
{
}

bool ComponentTrail::DrawInspector()
{
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();



	if (ImGui::CollapsingHeader("Trail System", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Vector of creation");
		
		ImGui::Spacing();
		
		
		if (ImGui::RadioButton("X", trail->vector == TrailVector::X))
		{
			trail->low = 4; trail->high = 5;
			trail->vector = TrailVector::X;
		} ImGui::SameLine();
		if (ImGui::RadioButton("Y", trail->vector == TrailVector::Y))
		{
			trail->low = 2; trail->high = 3;
			trail->vector = TrailVector::Y;
		} ImGui::SameLine();
		if (ImGui::RadioButton("Z", trail->vector == TrailVector::Z))
		{
			trail->low = 0; trail->high = 1;
			trail->vector = TrailVector::Z;
		}

		ImGui::Spacing();
		

		ImGui::DragFloat("Life Time", &trail->lifeTime, 10.0f, 0, 10000);
		
		
		ImGui::Spacing();
		ImGui::DragFloat("Min Distance", &trail->minDistance, 0.01f, 0.0f, 10.0f);
		
		ImGui::Spacing();
		

		ImGui::Checkbox("Create trail", &trail->create);
		ImGui::Spacing();
		

		ImGui::Checkbox("Orient trail", &trail->orient);
		ImGui::Spacing();
		

		if (ImGui::Checkbox("Use Custom Spawn", &trail->customSpawn))
		{
			if (trail->customSpawn)
			{
				trail->originalSpawnBox = trail->_spawnBox;
			}
		}
		ImGui::Spacing();
		
		if (trail->customSpawn)
		{
			math::float3 size = trail->originalSpawnBox.Size();
			if (ImGui::DragFloat3("Spawn Size", &size.x, 1.0f, 0.0f, 0.0f, "%.0f"))
				trail->SetSpawnSize(size);

		}


		ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "TRAIL MATERIAL: ");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Material");
		ImGui::SameLine(200, 15);

		if (trail->material != nullptr)
			ImGui::Button(trail->material->name.data(), { ImGui::GetWindowWidth() * 0.25F , 0 });
		else
			ImGui::Button("none", { ImGui::GetWindowWidth() * 0.25F , 0 });


		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
			if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
				FileNode* node = *(FileNode**)payload->Data;
				if (node != nullptr && node->type == FileDropType::MATERIAL) {
					std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
					path += "_meta.alien";
					u64 ID = App->resources->GetIDFromAlienPath(path.data());
					if (ID != 0) {
						ResourceMaterial* mat = (ResourceMaterial*)App->resources->GetResourceWithID(ID);
						if (mat != nullptr) {
							trail->SetMaterial(mat);
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();

		if (ImGui::Button("Delete", { ImGui::GetWindowWidth() * 0.15F , 0 }))
		{

			if (trail->material != nullptr) {
				trail->RemoveMaterial();
			}
		}


		if (trail->material != nullptr) {

			if (trail->material == trail->default_material)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}


			trail->material->DisplayMaterialOnInspector();


			if (trail->material == trail->default_material)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}
	}

	return true;
}

void ComponentTrail::SaveComponent(JSONArraypack* to_save)
{
}

void ComponentTrail::LoadComponent(JSONArraypack* to_load)
{
}

void ComponentTrail::Start()
{
	trail->Start();
}

void ComponentTrail::Stop()
{
	trail->Stop();
}

math::OBB ComponentTrail::GetOBBFromObject()
{
	return game_object_attached->GetGlobalOBB();
}

Trail* ComponentTrail::GetTrail()
{
	return trail;
}
