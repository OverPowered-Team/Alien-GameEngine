#include "CameraShake.h"

CameraShake::CameraShake() : Alien()
{
}

CameraShake::~CameraShake()
{
}

void CameraShake::Start()
{
}

void CameraShake::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_RETURN)) {
		Shake(0.13f);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_S)) {
		Shake(0.13f, 0.1f);
	}

	if (trauma > 0) {
		pre_off_set = off_set*Time::GetDT();
		shake_offset += pre_off_set;
		transform->SetGlobalRotation(preQuat);
		float3 rot = transform->GetGlobalRotation().ToEulerXYZ();
		float p = (Maths::PerlinNoise(0, shake_offset, shake_offset, 0.8) * 2 - 1);
		float yaw = rot.x + maxYaw * trauma * trauma * p;
		float pitch = rot.y + maxPitch * trauma * trauma * (Maths::PerlinNoise(1, shake_offset, shake_offset, 0.8) * 2 - 1);
		float roll = rot.z + maxRoll * trauma * trauma * (Maths::PerlinNoise(2, shake_offset, shake_offset, 0.8) * 2 - 1);

		transform->SetGlobalRotation(Quat::FromEulerXYZ(yaw,pitch,roll));

		trauma -= Time::GetDT() * traumaDecay * (trauma + 0.3f);
	}
	else {
		preQuat = transform->GetGlobalRotation();
		pre_off_set = off_set;
		traumaDecay = traumaDecayDef;
	}
}

void CameraShake::Shake(float strength, const float& traumaDecay)
{
	strength = Maths::Min(strength, 1.f);

	this->traumaDecay = traumaDecay;
	trauma += strength;
	trauma = Maths::Max(0.f, Maths::Min(trauma, 1.f));
}

void CameraShake::Shake(float strength)
{
	Shake(strength, traumaDecayDef);
}
