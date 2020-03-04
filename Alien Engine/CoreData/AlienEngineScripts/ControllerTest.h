#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API ControllerTest : public Alien {

public:

	ControllerTest();
	virtual ~ControllerTest();
	
	void Start();
	void Update();

public:

	float speed = 0;
	int controllerIndex = 0;
	GameObject* obj = nullptr;
	Prefab prefab;

};

ALIEN_FACTORY ControllerTest* CreateControllerTest() {
	ControllerTest* alien = new ControllerTest();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_SLIDER_INT(alien->controllerIndex, 1, 4);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->speed);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->obj);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->prefab);
	return alien;
} 
