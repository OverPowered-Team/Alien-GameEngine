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
	RGBA,
	UI_COLOR,
	UI_ALPHA,
	UI_RGBA
};

enum TweenType {
	notUsed = -1, linear, easeIn, easeOut, easeInOut, backIn, backOut, backInOut, bounceIn, bounceOut, bounceInOut,
	elasticIn, elasticOut, elasticInOut
};

class AnimTween: public Module
{
public:
	AnimTween() {};
	~AnimTween() {};

	update_status Update(float dt);

public:
	Tween* CreateTween(GameObject* gameObject, float time, TweenAction action, TweenType type);
	Tween* TweenMove(GameObject* gameObject, const float3& to, float time, TweenType type);
	Tween* TweenMoveTo(GameObject* gameObject, const float3& to, float time, TweenType type);
	Tween* TweenScale(GameObject* gameObject, const float3& to, float time, TweenType type);
	Tween* TweenRotate(GameObject* gameObject, const float3& to, float time, TweenType type);
	Tween* TweenColor(GameObject* gameObject, const float3& to, float time, TweenType type);
	Tween* TweenAlpha(GameObject* gameObject, const float to, float time, TweenType type);
	Tween* TweenRGBA(GameObject* gameObject, const float4& to, float time, TweenType type);

	Tween* TweenUIColor(GameObject* gameObject, const float3& to, float time, TweenType type);
	Tween* TweenUIAlpha(GameObject* gameObject, const float to, float time, TweenType type);
	Tween* TweenUIRGBA(GameObject* gameObject, const float4& to, float time, TweenType type);

private:
	std::vector<Tween*> tweens;
};
