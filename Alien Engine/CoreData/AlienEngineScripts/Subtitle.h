#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Subtitle : public Alien {

public:

	Subtitle();
	virtual ~Subtitle();
	
	void Start();
	void Update();

};

ALIEN_FACTORY Subtitle* CreateSubtitle() {
	Subtitle* alien = new Subtitle();
	// To show in inspector here

	return alien;
} 
