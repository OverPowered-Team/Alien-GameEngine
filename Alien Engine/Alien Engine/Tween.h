#pragma once

#include "AnimTween.h"
#include "MathGeoLib/include/Math/float4.h"

class ComponentTransform;

class Tween {

public:
	bool UpdateInternal();

private:
	void MoveInternal();
	void ScaleInternal();
	void RotateInternal();
	void ColorInternal();
	void AlphaInternal();
	void RGBAInternal();

	float4 LinealLerp();
	float4 EaseIn();
	float4 EaseOut();
	float4 EaseInOut();

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
