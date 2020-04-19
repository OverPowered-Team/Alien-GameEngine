#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TestScript : public Alien {

public:

	TestScript();
	virtual ~TestScript();
	
	void Start();
	void Update();


};

ALIEN_FACTORY TestScript* CreateTestScript() {
	TestScript* alien = new TestScript();
	// To show in inspector here

	return alien;
} 
