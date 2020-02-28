#pragma once

#include "UITween.h"
#include "MathGeoLib/include/Math/float4.h"

class ComponentTransform;

class TweenDescr {

public:

public:
	float4 from = { 0.0f,0.0f ,0.0f ,0.0f };
	float4 to = { 0.0f,0.0f ,0.0f ,0.0f };
	float time;
	UITweenAction type = NONE;
	ComponentTransform* trans = nullptr;

	float delay = 0.0f;
};
