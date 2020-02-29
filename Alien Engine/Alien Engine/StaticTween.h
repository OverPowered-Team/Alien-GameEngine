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
	};

	enum {
		notUsed = -1, linear
	};
public:
	static Tween* CreateTween(GameObject* gameObject, const float3& to, float time, int action, int type = 0);

};