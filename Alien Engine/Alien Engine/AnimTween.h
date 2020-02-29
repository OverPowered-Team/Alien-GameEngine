#pragma once

#include "Module.h"

class Tween;
class GameObject;

#define MAX_TWEENS 20

enum TweenAction {
	NONE = -1,
	MOVE_TO,
	MOVE,
	ROTATE,
	SCALE,
	ALPHA,
	COLOR,
};

enum TweenType {
	notUsed = -1, linear
};

class AnimTween: public Module
{
public:
	AnimTween() {};
	~AnimTween() {};

	update_status Update(float dt);

public:
	Tween* CreateTween(GameObject * gameObject, const float4 & to, float time, TweenAction action, TweenType type = linear );

private:
	std::vector<Tween*> tweens;
};
