#pragma once

#include "Module.h"

class Tween;
class GameObject;

enum TweenAction {
	NONE = -1,
	MOVE,
	ROTATE,
	SCALE,
	ALPHA,
	COLOR,
};

enum AnimTweenType {
	notUsed, linear
};

class AnimTween: Module
{
public:
	AnimTween() {};
	~AnimTween() {};

public:
	static Tween* Move(GameObject * gameObject, const float3 & to, float time);

public:
	static int maxTweens;

private:
	static Tween* tweens;
};
