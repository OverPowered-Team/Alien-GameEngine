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
		transform->SetGlobalRotation(preQuat);
		float3 rot = transform->GetGlobalRotation().ToEulerXYZ();

		float yaw = rot.x + maxYaw * trauma * trauma * Maths::PerlinNoise(0);
		float pitch = rot.y + maxPitch * trauma * trauma * Maths::PerlinNoise(0);
		float roll = rot.z + maxRoll * trauma * trauma * Maths::PerlinNoise(0);
		
		transform->SetGlobalRotation(Quat::FromEulerXYZ(yaw,pitch,roll));

		trauma -= Time::GetDT() * traumaDecay * (trauma + 0.3f);
	}
	else {
		preQuat = transform->GetGlobalRotation();
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
