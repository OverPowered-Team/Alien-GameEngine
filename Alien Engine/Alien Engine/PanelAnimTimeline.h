#pragma once

#include "Panel.h"
#include "ResourceAnimation.h"

class ComponentAnimator;

class PanelAnimTimeline : public Panel
{
public:

	PanelAnimTimeline(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	~PanelAnimTimeline();

	bool FillInfo();
	void PanelLogic();

public:

	ComponentAnimator* component_animator = nullptr;
	ResourceAnimation::Channel* channel = nullptr;
	ResourceAnimatorController* animator = nullptr;
	std::vector<ResourceAnimation*> animations;
	std::vector<float> num_frames_vector;
	

private:

	ImVec2 bar_mov = { 0, 0 };
	ImVec2 mouse_mov = { 0, 0 };

	float button_position = 0.0f;


	float offset = 0.0f;

	bool dragging = false;
	bool scrolled = false;

	float recSize = 1000;
	float zoom = 25;
	float speed = 0.5f;
	float progress = 0.0f;

	float windows_size = 1000.0f;

	bool play = false;
	bool pause = false;
	bool stop = false;

	float animation_time = 0.0f;

	bool changed = true;


};
