#pragma once

#include "Module.h"
#include "Globals.h"

class GameObject;
class ComponentImage;
class ComponentCanvas;

enum class FadeToBlackType
{
	FADE,
	DIAGONAL_1,
	DIAGONAL_2,

	NONE
};

enum class FadeType
{
	FADE_TO,
	FADE_FROM,
	COMPLETE_FADE,

	NONE
};

struct Fade
{
	Fade() {}

	// Fade Types
	FadeType fade_type = FadeType::NONE;
	FadeToBlackType ftb_type = FadeToBlackType::NONE;

	// Time values
	float fading_time = 0.0f;
	float time_start = 0.0f;
	float origin_value = 0.0f;
	float final_value = 1.0f;

	// Global Items
	GameObject* root_object = nullptr;
	float3 fade_color = { 0,0,0 };

	union
	{
		struct
		{
			ComponentImage* fading_image = nullptr;
		}linear_fade;

		struct
		{
			ComponentImage* diagonal_image_1 = nullptr;
			ComponentImage* diagonal_image_2 = nullptr;
		}diagonal_fade;
	};
};


class FadeToBlack : public Module
{
public:
	FadeToBlack(bool start_enabled = false);
	~FadeToBlack();

	update_status PreUpdate(float dt);

	void StartFade(float seconds, FadeType fade_type, FadeToBlackType FTB_Type, float3 fade_color);

	void Reset();

private:

	void CreateComponentImage(ComponentCanvas* canvas, ComponentImage** c_image, float3 color);

private:
	Fade* fade = nullptr;
	bool fading_from = false;
};
