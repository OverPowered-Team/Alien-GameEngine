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

	// Global Items
	GameObject* root_object = nullptr;

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

	void StartFade(float seconds, FadeToBlackType FTB_Type);

	void Reset();

private:

	void CreateComponentImage(ComponentCanvas* canvas, ComponentImage** c_image);

private: 
	Fade* fade = nullptr;

};
