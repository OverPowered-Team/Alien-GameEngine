#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Aasa : public Alien {

public:

	Aasa();
	virtual ~Aasa();
	
	void Start();
	void Update();

};

ALIEN_FACTORY Aasa* CreateAasa() {
	Aasa* alien = new Aasa();
	// To show in inspector here

	return alien;
} 
