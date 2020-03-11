#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Testanimator : public Alien {

public:

	Testanimator();
	virtual ~Testanimator();
	
	void Start();
	void Update();

	ComponentAnimator* anim = nullptr;
};

ALIEN_FACTORY Testanimator* CreateTestanimator() {
	Testanimator* alien = new Testanimator();
	// To show in inspector here

	return alien;
} 
