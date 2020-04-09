#pragma once

#include "Module.h"
#include "Globals.h"

class GameObject;
class ComponentImage;

enum class FadeToBlackType
{
	FADE,
	DIAGONAL,

	NONE
};

enum class FadeType
{
	FADE_TO,
	FADE_FROM,
	COMPLETE_FADE,

	NONE
};

class FadeToBlack : public Module
{
public:
	FadeToBlack(bool start_enabled = false);
	~FadeToBlack();

	update_status PreUpdate(float dt);

	void StartFade(float seconds, FadeToBlackType FTB_Type);

	void Reset();

private:
	float fading_time = 0.0f;
	FadeToBlackType ftb_type = FadeToBlackType::NONE;
	FadeType fade_type = FadeType::NONE;
	float time_start = 0.0f;
	// Items
	GameObject* fading_item = nullptr;
	GameObject* image = nullptr;
	ComponentImage* c_image = nullptr;
};
