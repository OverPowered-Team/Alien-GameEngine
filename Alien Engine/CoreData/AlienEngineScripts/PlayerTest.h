#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API PlayerTest : public Alien {

public:

	PlayerTest();
	virtual ~PlayerTest();
	
	void Start();
};

ALIEN_FACTORY PlayerTest* CreatePlayerTest() {
	PlayerTest* alien = new PlayerTest();
	// To show in inspector here

	return alien;
} 
