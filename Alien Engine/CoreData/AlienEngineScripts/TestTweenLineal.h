#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TestTweenLineal : public Alien {

public:

	TestTweenLineal();
	virtual ~TestTweenLineal();
	
	void Start();
	void Update();

};

ALIEN_FACTORY TestTweenLineal* CreateTestTweenLineal() {
	TestTweenLineal* alien = new TestTweenLineal();
	// To show in inspector here

	return alien;
} 
