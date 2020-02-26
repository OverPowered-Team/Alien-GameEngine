#pragma once

#include "Module.h"

class TweenDescr;

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
	static int maxTweens;

private:
	static TweenDescr* tweens;
};
