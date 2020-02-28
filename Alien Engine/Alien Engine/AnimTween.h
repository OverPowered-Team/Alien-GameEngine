#pragma once

#include "Module.h"

class Tween;
class GameObject;

#define MAX_TWEENS 20

enum TweenAction {
	NONE = -1,
	MOVE,
	ROTATE,
	SCALE,
	ALPHA,
	COLOR,
};

enum TweenType {
	notUsed = -1, linear
};

class AnimTween: Module
{
public:
	AnimTween() {};
	~AnimTween() {};

	void Update();

public:
	Tween* CreateTween(GameObject * gameObject, const float3 & to, float time, TweenAction action, TweenType type = linear );

private:
	std::vector<Tween*> tweens;
};
