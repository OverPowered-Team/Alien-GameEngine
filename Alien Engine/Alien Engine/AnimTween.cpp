#include "AnimTween.h"
#include "Tween.h"
#include "GameObject.h"
#include "ComponentTransform.h"


update_status AnimTween::Update(float dt)
{
	for (auto it_tween = tweens.begin(); it_tween != tweens.end();)
	{
		if (!(*it_tween)->UpdateInternal())
		{
			delete (*it_tween);
			it_tween = tweens.erase(it_tween);
		}
		else
			it_tween++;
	}
	return UPDATE_CONTINUE;
}

Tween* AnimTween::CreateTween(GameObject* gameObject, float time, TweenAction action, TweenType type)
{
	if (tweens.size() > MAX_TWEENS)
	{
		LOG_ENGINE("Max tweens %i has been reached. :(", MAX_TWEENS);
		return nullptr;
	}
		
	Tween* tween = new Tween();
	tween->trans = gameObject->transform;
	tween->time = time;
	tween->action = action;
	tween->type = type;

	tweens.push_back(tween);
	return tween;
}

Tween* AnimTween::TweenMove(GameObject* gameObject, const float3& to, float time, TweenType type)
{
	Tween* tween = CreateTween(gameObject, time, TweenAction::MOVE, type);
	if (tween)
	{
		tween->from = float4(gameObject->transform->GetGlobalPosition(), 0.0f);
		tween->to = tween->from + float4(to, 0.f);
	}
}

Tween* AnimTween::TweenMoveTo(GameObject* gameObject, const float3& to, float time, TweenType type)
{
	Tween* tween = CreateTween(gameObject, time, TweenAction::MOVE_TO, type);
	if (tween)
	{
		tween->from = float4(gameObject->transform->GetGlobalPosition(), 0.0f);
		tween->to = float4(to, 0.f);
	}
}

