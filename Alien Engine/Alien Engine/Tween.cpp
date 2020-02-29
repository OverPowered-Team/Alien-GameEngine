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
	case TweenAction::SCALE:
		ScaleInternal();
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

void Tween::ScaleInternal()
{
	float4 current_scale = float4::zero;

	switch (type)
	{
	case notUsed:
		break;
	case linear:
		current_scale = LinealLerp();
		break;
	default:
		break;
	}

	trans->SetGlobalScale(float3(current_scale.x, current_scale.y, current_scale.z));
}

float4 Tween::LinealLerp()
{
	float weight = current_time / time;
	return math::Lerp(from, to, weight);
}
