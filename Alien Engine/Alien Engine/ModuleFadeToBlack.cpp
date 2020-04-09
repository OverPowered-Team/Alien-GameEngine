#include "ModuleFadeToBlack.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentImage.h"
#include "ComponentTransform.h"
#include "AnimTween.h"
#include "Time.h"

FadeToBlack::FadeToBlack(bool start_enabled): Module(start_enabled)
{
	name.assign("FadeToBlack");
}

FadeToBlack::~FadeToBlack()
{
}

update_status FadeToBlack::PreUpdate(float dt)
{
	if (fade != nullptr)
	{
		float increment = (Time::GetTimeSinceStart() - fade->time_start) / fade->fading_time;

		switch (fade->ftb_type)
		{
		case FadeToBlackType::FADE:
		{
			fade->linear_fade.fading_image->SetBackgroundColor(0, 0, 0, math::Lerp(0.0f, 1.0f, increment));
			break;
		}
		case FadeToBlackType::DIAGONAL_1:
			break;
		}


		if ((fade->time_start + fade->fading_time) < Time::GetTimeSinceStart())
		{
			// Tween completed
			Reset();
		}
	}
	return UPDATE_CONTINUE;
}

void FadeToBlack::StartFade(float seconds, FadeToBlackType FTB_Type)
{
	if (FTB_Type != FadeToBlackType::NONE && fade == nullptr)
	{
		fade = new Fade();
		fade->fading_time = seconds;
		fade->ftb_type = FTB_Type;

		// Create Canvas
		fade->root_object = App->objects->CreateEmptyGameObject(nullptr, false);
		fade->root_object->SetName("FadeToBlack");
		App->objects->MoveObjectUp(fade->root_object, true);
		ComponentCanvas* canvas = new ComponentCanvas(fade->root_object);
		fade->root_object->AddComponent(canvas);

		// Fades
		switch (FTB_Type)
		{
		case FadeToBlackType::FADE:
		{
			CreateComponentImage(canvas, &fade->linear_fade.fading_image);
			break;
		}
		case FadeToBlackType::DIAGONAL_1:
		{
			CreateComponentImage(canvas, &fade->diagonal_fade.diagonal_image_1);
			CreateComponentImage(canvas, &fade->diagonal_fade.diagonal_image_2);
			break;
		}
		case FadeToBlackType::DIAGONAL_2:
		{
			CreateComponentImage(canvas, &fade->diagonal_fade.diagonal_image_1);
			CreateComponentImage(canvas, &fade->diagonal_fade.diagonal_image_2);
			break;
		}
		}
		
		// Enable Module to start PreUpdate
		fade->time_start = Time::GetTimeSinceStart();
		SetEnable(true);
	}
}

void FadeToBlack::Reset()
{
	GameObject::DestroyInstantly(fade->root_object);
	delete fade;
	fade = nullptr;
	SetEnable(false);
}

void FadeToBlack::CreateComponentImage(ComponentCanvas* canvas, ComponentImage** c_image)
{
	// Create Image and Set Component
	GameObject* image = new GameObject(fade->root_object);
	image->SetName("Fade");
	*c_image = new ComponentImage(image);
	image->AddComponent(*c_image);
	(*c_image)->SetBackgroundColor(0, 0, 0, 1);
	(*c_image)->SetCanvas(canvas);
	image->transform->SetLocalScale(float3(80, 45, 1));
}
