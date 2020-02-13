#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TestScript : public Alien {

public:

	TestScript();
	virtual ~TestScript();
	
	void Start();
	void Update();

	int num = 0;

};

ALIEN_FACTORY TestScript* CreateTestScript() {
	TestScript* alien = new TestScript();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->num);
	return alien;
} 
