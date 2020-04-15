#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CameraShake : public Alien {

public:

	CameraShake();
	virtual ~CameraShake();
	
	void Start();
	void Update();

	/*strength must be between 0 and 1*/
	void Shake(float strength);
	void Shake(float strength, const float& traumaDecay);

	float traumaDecayDef = 0.9f;

	float maxYaw = 1.f;
	float maxPitch = 1.f;
	float maxRoll = 1.f;

private:
	float trauma = 0.f;
	float traumaDecay = 0.9f;

	Quat preQuat = Quat::identity();
};

ALIEN_FACTORY CameraShake* CreateCameraShake() {
	CameraShake* alien = new CameraShake();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->traumaDecayDef, 0.001f, 1.0f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->maxYaw);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->maxPitch);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->maxRoll);
	

	return alien;
} 
