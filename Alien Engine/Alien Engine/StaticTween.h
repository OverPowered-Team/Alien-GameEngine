#pragma once

#include "GameObject.h"
#include "MathGeoLib/include/Math/float3.h"

enum TweenAction;

enum TweenType;

class __declspec(dllexport) Tween {
public:
	enum {
		NONE = -1,
		MOVE_TO,
		MOVE,
		ROTATE,
		SCALE,
		ALPHA,
		COLOR,
		RGBA
	};

	enum {
		notUsed = -1, linear, easeIn, easeOut, easeInOut
	};
public:
	static Tween* CreateTween(GameObject* gameObject, const float4& to, float time, int action, int type = 0);

};