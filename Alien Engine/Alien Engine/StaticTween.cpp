#include "Application.h"
#include "Tween.h"
#include "StaticTween.h"

Tween* Tween::TweenMove(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenMove(gameObject, to, time, (TweenType)type);
}

Tween* Tween::TweenMoveTo(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenMoveTo(gameObject, to, time, (TweenType)type);
}

Tween* Tween::TweenScale(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenScale(gameObject, to, time, (TweenType)type);
}

Tween* Tween::TweenRotate(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenRotate(gameObject, to, time, (TweenType)type);
}

Tween* Tween::TweenColor(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenColor(gameObject, to, time, (TweenType)type);
}

Tween* Tween::TweenAlpha(GameObject* gameObject, const float to, float time, int type)
{
	return App->tween->TweenAlpha(gameObject, to, time, (TweenType)type);
}

Tween* Tween::TweenRGBA(GameObject* gameObject, const float4& to, float time, int type)
{
	return App->tween->TweenRGBA(gameObject, to, time, (TweenType)type);
}

Tween* Tween::TweenUIColor(GameObject* gameObject, const float3& to, float time, int type)
{
	return App->tween->TweenUIColor(gameObject, to, time, (TweenType)type);
}

Tween* Tween::TweenUIAlpha(GameObject* gameObject, const float to, float time, int type)
{
	return App->tween->TweenUIAlpha(gameObject, to, time, (TweenType)type);;
}

Tween* Tween::TweenUIRGBA(GameObject* gameObject, const float4& to, float time, int type)
{
	return App->tween->TweenUIRGBA(gameObject, to, time, (TweenType)type);;
}
