#include "AnimTween.h"
#include "TweenDescr.h"
#include "GameObject.h"
#include "ComponentTransform.h"


Tween* AnimTween::Move(GameObject * gameObject, const float3 & to, float time)
{
	Tween* tween = nullptr;

	tween->to = float4(to, 0.f);
	tween->trans = gameObject;

	return Tween();
}
