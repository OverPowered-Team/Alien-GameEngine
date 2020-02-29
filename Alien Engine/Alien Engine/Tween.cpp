#include "Tween.h"
#include "Maths.h"
#include "ComponentTransform.h"
#include "Time.h"
#include "Tween.h"

bool Tween::UpdateInternal()
{
	bool ret = true;

	current_time += Time::GetDT();
	switch(action)
	{
	case TweenAction::MOVE:
		MoveInternal();
		break;
	case TweenAction::MOVE_TO:
		MoveInternal();
		break;
	}

	if (current_time > time)
		ret = false;

	return ret;
}

void Tween::MoveInternal()
{
	float4 current_pos = float4::zero;

	switch (type)
	{
	case notUsed:
		break;
	case linear:
		current_pos = LinealLerp();
		break;
	default:
		break;
	}

	trans->SetGlobalPosition(float3(current_pos.x, current_pos.y, current_pos.z));
}

float4 Tween::LinealLerp()
{
	float weight = current_time / time;
	return math::Lerp(from, to, weight);
}
