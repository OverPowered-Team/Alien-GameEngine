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
	RGBA
};

enum TweenType {
	notUsed = -1, linear, easeIn, easeOut, easeInOut
};

class AnimTween: public Module
{
public:
	AnimTween() {};
	~AnimTween() {};

	update_status Update(float dt);

public:
	Tween* CreateTween(GameObject* gameObject, float time, TweenAction action, TweenType type);
	Tween* TweenMove();

private:
	std::vector<Tween*> tweens;
};
