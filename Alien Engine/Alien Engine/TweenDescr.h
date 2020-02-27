#pragma once

#include "UITween.h"
#include "MathGeoLib/include/Math/float3.h"

class ComponentTransform;

class TweenDescr {

public:
	bool init();
	bool updateInternal();

	void callOnCompletes();

public:
	float3 from;
	float3 to;
	float time;
	float direction;
	UITweenAction type;
	ComponentTransform* trans;

	float delay = 0.0f;
	bool hasInitiliazed = false;
	float passed = 0.0f;
	float ratioPassed = 0.0f;

	static float val;
	static float dt;
	static float3 newVect;


};
