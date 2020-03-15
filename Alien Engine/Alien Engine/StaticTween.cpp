#include "Application.h"
#include "StaticTween.h"

ModuleTween* ModuleTween::TweenMove(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenMove(gameObject, to, time, (TweenType)type);
}

ModuleTween* ModuleTween::TweenMoveTo(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenMoveTo(gameObject, to, time, (TweenType)type);
}

ModuleTween* ModuleTween::TweenScale(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenScale(gameObject, to, time, (TweenType)type);
}

ModuleTween* ModuleTween::TweenRotate(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenRotate(gameObject, to, time, (TweenType)type);
}

ModuleTween* ModuleTween::TweenColor(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenColor(gameObject, to, time, (TweenType)type);
}

ModuleTween* ModuleTween::TweenAlpha(GameObject* gameObject, const float to, float time, int type)
{
	return App->tween->TweenAlpha(gameObject, to, time, (TweenType)type);
}

ModuleTween* ModuleTween::TweenRGBA(GameObject* gameObject, const float4& to, float time, int type)
{
	return App->tween->TweenRGBA(gameObject, to, time, (TweenType)type);
}

ModuleTween* ModuleTween::TweenUIColor(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenUIColor(gameObject, to, time, (TweenType)type);
}

ModuleTween* ModuleTween::TweenUIAlpha(GameObject* gameObject, const float to, float time, int type)
{
	return App->tween->TweenUIAlpha(gameObject, to, time, (TweenType)type);;
}

ModuleTween* ModuleTween::TweenUIRGBA(GameObject* gameObject, const float4& to, float time, int type)
{
	return App->tween->TweenUIRGBA(gameObject, to, time, (TweenType)type);;
}
