#pragma once

#include "Module.h"

class TweenDescr;
class GameObject;

enum TweenType {
	MOVE_X,
	MOVE_Y,
	MOVE_Z,
	MOVE_LOCAL_X,
	MOVE_LOCAL_Y,
	MOVE_LOCAL_Z,
	ROTATE_X,
	ROTATE_Y,
	ROTATE_Z,
	SCALE_X,
	SCALE_Y,
	SCALE_Z
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
