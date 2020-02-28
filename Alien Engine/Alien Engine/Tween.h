#pragma once

#include "AnimTween.h"
#include "MathGeoLib/include/Math/float4.h"

class ComponentTransform;

class Tween {

public:
	bool UpdateInternal();

private:
	void MoveInternal();

	float4 LinealLerp();

public:
	float4 from = { 0.0f,0.0f ,0.0f ,0.0f };
	float4 to = { 0.0f,0.0f ,0.0f ,0.0f };
	float time;
	float current_time = 0.F;
	TweenAction action = TweenAction::NONE;
	TweenType type = TweenType::notUsed;
	ComponentTransform* trans = nullptr;

	float delay = 0.0f;
};
