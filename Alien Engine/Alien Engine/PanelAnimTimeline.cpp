#include "Application.h"
#include "ModuleObjects.h"
#include "ComponentAnimator.h"
#include "ResourceAnimatorController.h"
#include "PanelAnimTimeline.h"
#include "mmgr/mmgr.h"

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

	animations.clear();
	num_frames_vector.clear();
	component_animator = nullptr;
	animator = nullptr;
	if (App->objects->GetSelectedObjects().size() > 0)	
	{

		std::list<GameObject*> selected = App->objects->GetSelectedObjects();
		auto go = selected.begin();

		for (; go != selected.end(); ++go)
		{
			if (*go != nullptr && (*go)->GetComponent(ComponentType::ANIMATOR))
			{
				component_animator = (ComponentAnimator*)(*go)->GetComponent(ComponentType::ANIMATOR);

				if (component_animator->GetResourceAnimatorController() != nullptr)	
				{
					animator = component_animator->GetResourceAnimatorController();

					for (uint i = 0; i < animator->GetNumStates(); ++i)
					{
						if (animator->GetStates()[i]->GetClip())
						{
							animations.push_back(animator->GetStates()[i]->GetClip());
							num_frames_vector.push_back(animator->GetStates()[i]->GetClip()->GetDuration());
							ret = true;
							changed = false;
						}
						
					}
				}
			}
		}
	}
	return ret;
}


void PanelAnimTimeline::PanelLogic()
{
	ImGuiWindowFlags aboutFlags = 0;
	aboutFlags |= ImGuiWindowFlags_HorizontalScrollbar;
	ImGui::Begin("Animation Timeline", &enabled, aboutFlags);

	if (changed)
		FillInfo();
	if (!changed)
	{
		//Animation bar Progress
		ImGui::SetCursorPosX(85);
		ImGui::ProgressBar(animation_time / (animations.back()->GetDuration() / animations.back()->ticks_per_second), { windows_size,0 });

		//Animation types of Keys
		ImGui::BeginGroup();

		ImGui::SetCursorPosY(85);

		ImGui::Text("Position");

		ImGui::SetCursorPosY(125);

		ImGui::Text("Rotation");

		ImGui::SetCursorPosY(165);

		ImGui::Text("Scale");

		ImGui::EndGroup();

		ImGui::SameLine();

		//Animation TimeLine
		ImGui::BeginChild("TimeLine", ImVec2(windows_size, 180), true, ImGuiWindowFlags_HorizontalScrollbar);
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImVec2 redbar = ImGui::GetCursorScreenPos();
		/*ImGui::InvisibleButton("scrollbar", { num_frames_vector.back() * zoom ,140 });*/
		ImGui::SetCursorScreenPos(p);

		for (int i = 0; i < num_frames_vector.back(); i++)
		{
			ImGui::BeginGroup();

			ImGui::GetWindowDrawList()->AddLine({ p.x,p.y }, ImVec2(p.x, p.y + 135), IM_COL32(100, 100, 100, 255), 1.0f);

			char frame[8];
			sprintf(frame, "%01d", i);
			ImVec2 aux = { p.x + 3,p.y };
			ImGui::GetWindowDrawList()->AddText(aux, ImColor(1.0f, 1.0f, 1.0f, 1.0f), frame);

			if (animations.back() != nullptr && channel != nullptr)
			{
				if (channel->position_keys->time == i)
				{
					ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 35), 6.0f, ImColor(1.0f, 0.0f, 0.0f, 0.5f));
				}
				if (channel->rotation_keys->time == i)
				{
					ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 75), 6.0f, ImColor(0.0f, 1.0f, 0.0f, 0.5f));
				}
				if (channel->scale_keys->time == i)
				{
					ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 115), 6.0f, ImColor(0.0f, 0.0f, 1.0f, 0.5f));
				}
			}

			p = { p.x + zoom,p.y };

			ImGui::EndGroup();

			ImGui::SameLine();

		}

		//RedLine 
		if (!play)
		{
			ImGui::GetWindowDrawList()->AddLine({ redbar.x,redbar.y - 10 }, ImVec2(redbar.x, redbar.y + 165), IM_COL32(255, 0, 0, 100), 1.0f);
			progress = 0.0f;
			ImGui::SetScrollX(0);
		}
		else
		{
			float aux_progression_bar = progress;

			ImGui::GetWindowDrawList()->AddLine({ redbar.x + progress,redbar.y - 10 }, ImVec2(redbar.x + progress, redbar.y + 165), IM_COL32(255, 0, 0, 255), 1.0f);

			if (!pause)
			{
				progress = (animations.back()->GetDuration() * animations.back()->ticks_per_second) * zoom;
				scrolled = false;
			}

			if (progress != 0 && progress > windows_size && !scrolled)
			{
				float scroolPos = ImGui::GetScrollX();
				ImGui::SetScrollX(scroolPos + windows_size);
				scrolled = true;
			}

			if (aux_progression_bar > progress)
			{
				progress = 0.0f;
				ImGui::SetScrollX(0);
			}
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();

		ImGui::BeginChild("All Animations", ImVec2(250, 140), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

		if (animations.back() != nullptr)
		{
			for (int i = 0; i < animations.back()->num_channels; i++)
			{
				if (ImGui::Button(animations.back()->channels[i].name.c_str()))
				{
					channel = &animations.back()->channels[i];
				}
			}
		}

		ImGui::EndChild();

		ImGui::BeginChild("Selected Bone", ImVec2(250, 30), true);

		if (channel != nullptr)
		{
			ImGui::Text(channel->name.c_str());
		}

		ImGui::EndChild();
		ImGui::EndGroup();

		ImGui::NewLine();
	}

	ImGui::End();
}