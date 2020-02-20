#include "Application.h"
#include "ModuleObjects.h"
#include "PanelAnimTimeline.h"

PanelAnimTimeline::PanelAnimTimeline(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Animation Timeline", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);

}

PanelAnimTimeline::~PanelAnimTimeline()
{
}

bool PanelAnimTimeline::FillInfo()
{
	bool ret = false;

	/*if (App->objects->GetSelectedObjects().size() > 0)	{

		std::list<GameObject*> selected = App->objects->GetSelectedObjects();
		auto item = selected.begin();

		for (; item != selected.end(); ++item) {
			if (*item != nullptr && *item->GetAnimation()) {

				component_animation = (ComponentAnimation*)App->scene->GetSelected()->GetAnimation();

				if (component_animation->resource_animation != nullptr)
				{
					num_frames = component_animation->resource_animation->duration;

					ret = true;
				}
			}
		}
	}*/

	return ret;
}


void PanelAnimTimeline::PanelLogic()
{
	ImGuiWindowFlags aboutFlags = 0;
	aboutFlags |= ImGuiWindowFlags_HorizontalScrollbar;
	ImGui::Begin("Animation Timeline", &enabled, aboutFlags);

	if (FillInfo())
	{
	//	//Animation bar Progress
	//	ImGui::SetCursorPosX(85);
	//	ImGui::ProgressBar((component_animation->animation_time / (component_animation->resource_animation->duration / component_animation->resource_animation->ticks_per_second)), { windows_size,0 });

	//	//Animation typeos of Keys
	//	ImGui::BeginGroup();

	//	ImGui::SetCursorPosY(85);

	//	ImGui::Text("Position");

	//	ImGui::SetCursorPosY(125);

	//	ImGui::Text("Rotation");

	//	ImGui::SetCursorPosY(165);

	//	ImGui::Text("Scale");

	//	ImGui::EndGroup();

	//	ImGui::SameLine();

	//	//Animation TimeLine
	//	ImGui::BeginChild("TimeLine", ImVec2(windows_size, 180), true, ImGuiWindowFlags_HorizontalScrollbar);
	//	ImVec2 p = ImGui::GetCursorScreenPos();
	//	ImVec2 redbar = ImGui::GetCursorScreenPos();
	//	ImGui::InvisibleButton("scrollbar", { num_frames * zoom ,140 });
	//	ImGui::SetCursorScreenPos(p);

	//	for (int i = 0; i < num_frames; i++)
	//	{
	//		ImGui::BeginGroup();

	//		ImGui::GetWindowDrawList()->AddLine({ p.x,p.y }, ImVec2(p.x, p.y + 135), IM_COL32(100, 100, 100, 255), 1.0f);

	//		char frame[8];
	//		sprintf(frame, "%01d", i);
	//		ImVec2 aux = { p.x + 3,p.y };
	//		ImGui::GetWindowDrawList()->AddText(aux, ImColor(1.0f, 1.0f, 1.0f, 1.0f), frame);

	//		if (component_animation->resource_animation != nullptr && node_animation != nullptr)
	//		{
	//			if (node_animation->position_keys_times[i] == i)
	//			{
	//				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 35), 6.0f, ImColor(1.0f, 0.0f, 0.0f, 0.5f));
	//			}
	//			if (node_animation->rotation_keys_times[i] == i)
	//			{
	//				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 75), 6.0f, ImColor(0.0f, 1.0f, 0.0f, 0.5f));
	//			}
	//			if (node_animation->scale_keys_times[i] == i)
	//			{
	//				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 115), 6.0f, ImColor(0.0f, 0.0f, 1.0f, 0.5f));
	//			}
	//		}

	//		p = { p.x + zoom,p.y };

	//		ImGui::EndGroup();

	//		ImGui::SameLine();

	//	}

	//	//RedLine 
	//	if (!App->timemanager->play)
	//	{
	//		ImGui::GetWindowDrawList()->AddLine({ redbar.x,redbar.y - 10 }, ImVec2(redbar.x, redbar.y + 165), IM_COL32(255, 0, 0, 100), 1.0f);
	//		progress = 0.0f;
	//		ImGui::SetScrollX(0);
	//	}
	//	else
	//	{
	//		float aux_progression_bar = progress;

	//		ImGui::GetWindowDrawList()->AddLine({ redbar.x + progress,redbar.y - 10 }, ImVec2(redbar.x + progress, redbar.y + 165), IM_COL32(255, 0, 0, 255), 1.0f);

	//		if (!App->timemanager->pause)
	//		{
	//			progress = (component_animation->animation_time * component_animation->resource_animation->ticks_per_second) * zoom;
	//			scrolled = false;
	//		}

	//		if (progress != 0 && progress > windows_size && !scrolled)
	//		{
	//			float scroolPos = ImGui::GetScrollX();
	//			ImGui::SetScrollX(scroolPos + windows_size);
	//			scrolled = true;
	//		}

	//		if (aux_progression_bar > progress)
	//		{
	//			progress = 0.0f;
	//			ImGui::SetScrollX(0);
	//		}
	//	}

	//	ImGui::EndChild();

	//	ImGui::SameLine();

	//	ImGui::BeginGroup();

	//	ImGui::BeginChild("All Animations", ImVec2(250, 140), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

	//	if (component_animation->resource_animation != nullptr)
	//	{
	//		for (int i = 0; i < component_animation->resource_animation->num_channels; i++)
	//		{
	//			if (ImGui::Button(component_animation->resource_animation->nodes[i].name_node.c_str()))
	//			{
	//				node_animation = &component_animation->resource_animation->nodes[i];
	//			}
	//		}
	//	}

	//	ImGui::EndChild();

	//	ImGui::BeginChild("Selected Bone", ImVec2(250, 30), true);

	//	if (node_animation != nullptr)
	//	{
	//		ImGui::Text(node_animation->name_node.c_str());
	//	}

	//	ImGui::EndChild();
	//	ImGui::EndGroup();

	//	ImGui::NewLine();
	}

	ImGui::End();
}