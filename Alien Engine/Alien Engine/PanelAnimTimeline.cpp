#include "Application.h"
#include "ModuleObjects.h"
#include "ComponentAnimator.h"
#include "ResourceAnimatorController.h"
#include "Time.h"
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

	animations.clear();
	current_animation = nullptr;
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
	{
		FillInfo();
		ImGui::Text("ANIMATION NOT SELECTED");
		setted = false;
		
	}
	else
	{
		if (!setted)
		{
			current_animation = animations[0];
			channel = &current_animation->channels[0];
			setted = true;
		}
		
		current_num_frames = current_animation->end_tick - current_animation->start_tick;

		if (Time::IsPlaying() && !play)
		{
			play = true;
			aux_time = Time::GetTimeSinceStart();
			pause = false;
			stop_in_game = true;
		}
		else if (Time::IsPlaying() && pause)
		{
			aux_time = Time::GetTimeSinceStart() - animation_time;
			pause = false;
		}
		else if (Time::IsPaused())
		{
			pause = true;
			aux_time = animation_time;
		}
		else if (!Time::IsInGameState() && stop_in_game)
		{
			play = false;
			animation_time = 0.0f;
			pause = false;
			stop = true;
			stop_in_game = false;
		}
		else
		{
			// Buttons Play
			if (ImGui::Button("Play"))
			{
				if (!pause)
				{
					aux_time = Time::GetTimeSinceStart();
				}
				else
				{
					aux_time = Time::GetTimeSinceStart() - animation_time;
				}
				play = true;
				pause = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause"))
			{
				pause = !pause;
				if (!pause)
					aux_time = Time::GetTimeSinceStart() - animation_time;
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop") && play)
			{
				play = false;
				animation_time = 0.0f;
				pause = false;
				stop = true;
			}

			ImGui::SameLine();
		}
		

		//Animation bar Progress
		ImGui::SetCursorPosX(165);
		ImGui::ProgressBar(animation_time / current_animation->GetDuration(), { windows_size,0 });


		//Choose Animations
		ImGui::BeginGroup();

		ImGui::BeginChild("All Animations", ImVec2(150, 140), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

		
		for (int i = 0; i < animations.size(); i++)
		{
			if (ImGui::Button(animations[i]->name.c_str()))
			{
				current_animation = animations[i];
				current_num_frames = current_animation->end_tick - current_animation->start_tick;
			}
		}
		
		ImGui::EndChild();

		ImGui::BeginChild("Selected Animation", ImVec2(150, 30), true);

		if (current_animation != nullptr)
		{
			ImGui::Text(current_animation->name.c_str());
		}

		ImGui::EndChild();
		ImGui::EndGroup();

		ImGui::SameLine();

		//ALL TIMELINE
		ImGui::BeginChild("Timeline", ImVec2(windows_size, 170), true);

		//Animation types of Keys
		ImGui::BeginGroup();
		ImGui::SetCursorPosY(45);

		ImGui::Text("Position");

		ImGui::SetCursorPosY(85);

		ImGui::Text("Rotation");

		ImGui::SetCursorPosY(125);

		ImGui::Text("Scale");

		ImGui::EndGroup();

		ImGui::SameLine();

		//Animation TimeLine
		ImGui::BeginChild("TimeLine", ImVec2(windows_size - 80, 150), true, ImGuiWindowFlags_HorizontalScrollbar);
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImVec2 redbar = ImGui::GetCursorScreenPos();
		ImGui::InvisibleButton("scrollbar", { current_num_frames * zoom + zoom,140 });
		ImGui::SetCursorScreenPos(p);

		for (int i = 0; i <= current_num_frames; i++)
		{
			ImGui::BeginGroup();

			ImGui::GetWindowDrawList()->AddLine({ p.x,p.y + 15 }, ImVec2(p.x, p.y + 135), IM_COL32(100, 100, 100, 255), 1.0f);

			char frame[8];
			sprintf(frame, "%01d", i);
			ImVec2 aux = { p.x - 4,p.y };
			ImGui::GetWindowDrawList()->AddText(aux, ImColor(1.0f, 1.0f, 1.0f, 1.0f), frame);

			if (current_animation != nullptr && channel != nullptr)
			{
				if (channel->position_keys[i].time == i)
				{
					ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 35), 6.0f, ImColor(1.0f, 0.0f, 0.0f, 0.5f));
				}
				if (channel->rotation_keys[i].time == i)
				{
					ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 75), 6.0f, ImColor(0.0f, 1.0f, 0.0f, 0.5f));
				}
				if (channel->scale_keys[i].time == i)
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
			ImGui::GetWindowDrawList()->AddLine({ redbar.x,redbar.y - 10 }, ImVec2(redbar.x, redbar.y + 135), IM_COL32(255, 0, 0, 100), 1.0f);
			progress = 0.0f;
			
			if (stop)
			{
				ImGui::SetScrollX(0);
				stop = false;
			}
		}
		else
		{

			ImGui::GetWindowDrawList()->AddLine({ redbar.x + progress,redbar.y - 10 }, ImVec2(redbar.x + progress, redbar.y + 135), IM_COL32(255, 0, 0, 255), 1.0f);

			if (!pause)
			{
				animation_time = Time::GetTimeSinceStart() - aux_time;
				progress = (animation_time * current_animation->ticks_per_second) * zoom;
			}

			if (progress != 0 && progress > windows_size + ImGui::GetScrollX())
			{
				ImGui::SetScrollX(progress);

			}

			if (animation_time > current_animation->GetDuration())
			{
				progress = 0.0f;
				ImGui::SetScrollX(0);
				aux_time = Time::GetTimeSinceStart();
			}
			
		}

		ImGui::EndChild();
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();

		ImGui::BeginChild("All Bones", ImVec2(150, 140), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

		if (current_animation != nullptr)
		{
			for (int i = 0; i < current_animation->num_channels; i++)
			{
				if (ImGui::Button(current_animation->channels[i].name.c_str()))
				{
					channel = &current_animation->channels[i];
				}
			}
		}

		ImGui::EndChild();

		ImGui::BeginChild("Selected Bone", ImVec2(150, 30), true);

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