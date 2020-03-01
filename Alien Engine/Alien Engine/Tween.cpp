#include "Tween.h"
#include "Maths.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
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
	case TweenAction::ROTATE:
		RotateInternal();
		break;
	case TweenAction::COLOR:
		ColorInternal();
		break;
	case TweenAction::ALPHA:
		AlphaInternal();
		break;
	case TweenAction::RGBA:
		RGBAInternal();
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
	case easeIn:
		current_pos = EaseIn();
		break;
	case easeOut:
		current_pos = EaseOut();
		break;
	case easeInOut:
		current_pos = EaseInOut();
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
	case easeIn:
		current_scale = EaseIn();
		break;
	case easeOut:
		current_scale = EaseOut();
		break;
	case easeInOut:
		current_scale = EaseInOut();
		break;
	default:
		break;
	}

	//trans->SetGlobalScale(float3(current_scale.x, current_scale.y, current_scale.z));
}

void Tween::RotateInternal()
{
	float4 current_rotation = float4::zero;

	switch (type)
	{
	case notUsed:
		break;
	case linear:
		current_rotation = LinealLerp();
		break;
	case easeIn:
		current_rotation = EaseIn();
		break;
	case easeOut:
		current_rotation = EaseOut();
		break;
	case easeInOut:
		current_rotation = EaseInOut();
		break;
	default:
		break;
	}

	//trans->SetGlobalRotation(float3(current_scale.x, current_scale.y, current_scale.z));
}

void Tween::ColorInternal()
{
	float4 current_color = float4::zero;

	switch (type)
	{
	case notUsed:
		break;
	case linear:
		current_color = LinealLerp();
		break;
	case easeIn:
		current_color = EaseIn();
		break;
	case easeOut:
		current_color = EaseOut();
		break;
	case easeInOut:
		current_color = EaseInOut();
		break;
	default:
		break;
	}

	ComponentMaterial* material = trans->game_object_attached->GetComponent<ComponentMaterial>();
	if(material)
		material->color = {current_color.x, current_color.y, current_color.z, material->color.a};
}

void Tween::AlphaInternal()
{
	float4 current_alpha = float4::zero;

	switch (type)
	{
	case notUsed:
		break;
	case linear:
		current_alpha = LinealLerp();
		break;
	case easeIn:
		current_alpha = EaseIn();
		break;
	case easeOut:
		current_alpha = EaseOut();
		break;
	case easeInOut:
		current_alpha = EaseInOut();
		break;
	default:
		break;
	}

	ComponentMaterial* material = trans->game_object_attached->GetComponent<ComponentMaterial>();
	if (material)
		material->color.a = current_alpha.w;
}

void Tween::RGBAInternal()
{
	float4 current_rgba = float4::zero;

	switch (type)
	{
	case notUsed:
		break;
	case linear:
		current_rgba = LinealLerp();
		break;
	case easeIn:
		current_rgba = EaseIn();
		break;
	case easeOut:
		current_rgba = EaseOut();
		break;
	case easeInOut:
		current_rgba = EaseInOut();
		break;
	default:
		break;
	}

	ComponentMaterial* material = trans->game_object_attached->GetComponent<ComponentMaterial>();
	if (material)
		material->color = { current_rgba.x, current_rgba.y, current_rgba.z, current_rgba.w };
}

float4 Tween::LinealLerp()
{
	float weight = current_time / time;
	return math::Lerp(from, to, weight);
}

float4 Tween::EaseIn()
{
	float aux_time = current_time / time;
	return (to - from) * aux_time * aux_time + from;
}

float4 Tween::EaseOut()
{
	float aux_time = current_time / time;
	return -(to - from) * aux_time * (aux_time - 2) + from;
}

float4 Tween::EaseInOut()
{
	float aux_time = current_time / (time * 0.5);
	if (aux_time < 1) 
		return (to - from) / 2 * aux_time * aux_time + from;
	aux_time--;
	return -(to - from) / 2 * (aux_time * (aux_time - 2) - 1) + from;
}
