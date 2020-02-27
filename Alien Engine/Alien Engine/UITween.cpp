#include "UITween.h"
#include "TweenDescr.h"
#include "GameObject.h"
#include "ComponentTransform.h"

TweenDescr* UITween::move(GameObject* gameObject, float3 to, float time)
{
	return pushNewTween(gameObject, to, time, options().setCanvasMove().setRect(rectTrans));
}

TweenDescr* UITween::pushNewTween(GameObject* gameObject, float3 to, float time, TweenDescr* tween)
{
	//init(maxTweens);
	if (gameObject == nullptr || tween == nullptr)
		return nullptr;

	tween->transform = gameObject.GetComponent<ComponentTransform*>();
	tween.to = to;
	tween.time = time;

	if (tween.time <= 0.0f)
		tween.updateInternal();
	//tween.hasPhysics = gameObject.rigidbody!=null;

	return tween;
}
