#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API PlayerTest : public Alien {

public:

	PlayerTest();
	virtual ~PlayerTest();
	
	void Start();
	void Update();

	GameObject* speed = 0;

	void OnDrawGizmos();

};

ALIEN_FACTORY PlayerTest* CreatePlayerTest() {
	PlayerTest* alien = new PlayerTest();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->speed);

	return alien;
} 
