#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Test_curves : public Alien {

public:

	Test_curves();
	virtual ~Test_curves();
	
	void Start();
	void Update();

	GameObject* gocurve = nullptr;
	ComponentCurve* curve = nullptr;
	float actual_pos = 0.f;
	float speed = 1.f;
	bool first_frame = true;
	bool reset = false;
};

ALIEN_FACTORY Test_curves* CreateTest_curves() {
	Test_curves* alien = new Test_curves();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->speed);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->gocurve);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->reset);

	return alien;
} 
