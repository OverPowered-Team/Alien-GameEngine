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
	float increment = (Time::GetTimeSinceStart()-time_start)/fading_time;
	c_image->SetBackgroundColor(0, 0, 0, math::Lerp(0.0f, 1.0f, increment));
	if ((time_start + fading_time) < Time::GetTimeSinceStart())
	{
		// Tween completed
		Reset();
	}

	return UPDATE_CONTINUE;
}

void FadeToBlack::StartFade(float seconds, FadeToBlackType FTB_Type)
{
	if (FTB_Type != FadeToBlackType::NONE)
	{
		fading_time = seconds;
		ftb_type = FTB_Type;

		// Create Canvas
		fading_item = App->objects->CreateEmptyGameObject(nullptr, false);
		App->objects->MoveObjectUp(fading_item, true);
		ComponentCanvas* canvas = new ComponentCanvas(fading_item);
		fading_item->AddComponent(canvas);

		// Fades
		switch (FTB_Type)
		{
		case FadeToBlackType::FADE:
		{
			// Create Image
			image = new GameObject(fading_item);
			c_image = new ComponentImage(image);
			image->AddComponent(c_image);
			c_image->SetBackgroundColor(0, 0, 0, 0);
			c_image->SetCanvas(canvas);
			image->transform->SetLocalScale(float3(80, 50, 1));

			break;
		}
		case FadeToBlackType::DIAGONAL:
		{
			break;
		}
		}
		
		// Enable Module to start PreUpdate
		time_start = Time::GetTimeSinceStart();
		SetEnable(true);
	}
}

void FadeToBlack::Reset()
{
	GameObject::DestroyInstantly(fading_item);
	SetEnable(false);
}
