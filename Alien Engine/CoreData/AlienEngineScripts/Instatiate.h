#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Instatiate : public Alien {

public:

	Instatiate();
	virtual ~Instatiate();
	
	void Start();
	void Update();

public:
	Prefab prefab;
};

ALIEN_FACTORY Instatiate* CreateInstatiate() {
	Instatiate* alien = new Instatiate();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->prefab);

	return alien;
} 
