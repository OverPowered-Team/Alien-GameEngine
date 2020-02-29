#include "Application.h"
#include "StaticTween.h"

Tween* Tween::CreateTween(GameObject* gameObject, const float3& to, float time, int action, int type)
{
	return App->tween->CreateTween(gameObject, to, time, (TweenAction)action, (TweenType)type);
}
