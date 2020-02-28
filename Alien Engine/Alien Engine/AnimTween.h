#pragma once

#include "Module.h"

class TweenDescr;
class GameObject;

enum TweenAction {
	NONE = -1,
	MOVE,
	ROTATE,
	SCALE,
	ALPHA,
	COLOR,
};

enum UITweenType {
	notUsed, linear
};

class UITween: Module
{
public:
	UITween() {};
	~UITween() {};

public:
	static TweenDescr move(GameObject gameObject, float3 to, float time);
	
private:
	static TweenDescr pushNewTween(GameObject gameObject, float3 to, float time, TweenDescr tween);

public:
	static int maxTweens;

private:
	static TweenDescr* tweens;
};
