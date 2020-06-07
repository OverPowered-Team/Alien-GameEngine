#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TestFollowCurve : public Alien {

public:

	TestFollowCurve();
	virtual ~TestFollowCurve();
	
	void Start();
	void Update();

	GameObject* curve = nullptr;

	float speed = 0.1f;
	float at = 0.f;
	bool reset = false;
};

ALIEN_FACTORY TestFollowCurve* CreateTestFollowCurve() {
	TestFollowCurve* alien = new TestFollowCurve();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->curve);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->speed);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->reset);

	return alien;
} 
