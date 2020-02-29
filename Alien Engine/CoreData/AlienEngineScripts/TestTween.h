#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TestTween : public Alien {

public:

	TestTween();
	virtual ~TestTween();
	
	void Start();
	void Update();

};

ALIEN_FACTORY TestTween* CreateTestTween() {
	TestTween* alien = new TestTween();
	// To show in inspector here
	return alien;
} 
