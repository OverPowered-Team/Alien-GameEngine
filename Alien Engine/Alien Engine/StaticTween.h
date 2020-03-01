#pragma once

#include "GameObject.h"
#include "MathGeoLib/include/Math/float3.h"

enum TweenType;

class __declspec(dllexport) Tween {
public:

	enum {
		notUsed = -1, linear, easeIn, easeOut, easeInOut
	};

public:
	static Tween* TweenMove(GameObject* gameObject, const float3& to, float time, int type = 0);
	static Tween* TweenMoveTo(GameObject* gameObject, const float3& to, float time, int type = 0);
	static Tween* TweenScale(GameObject* gameObject, const float3& to, float time, int type = 0);
	static Tween* TweenRotate(GameObject* gameObject, const float3& to, float time, int type = 0);
	// Color values has to go from 0 to 1 
	static Tween* TweenColor(GameObject* gameObject, const float3& to, float time, int type = 0);
	static Tween* TweenAlpha(GameObject* gameObject, const float to, float time, int type = 0);
	static Tween* TweenRGBA(GameObject* gameObject, const float4& to, float time, int type = 0);

};