#pragma once

#include "AnimTween.h"
#include "MathGeoLib/include/Math/float4.h"

class ComponentTransform;

class Tween {

public:

public:
	float4 from = { 0.0f,0.0f ,0.0f ,0.0f };
	float4 to = { 0.0f,0.0f ,0.0f ,0.0f };
	float time;
	TweenAction type = NONE;
	ComponentTransform* trans = nullptr;

	float delay = 0.0f;
};
