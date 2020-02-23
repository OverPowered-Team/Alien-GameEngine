#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API JsonCreate : public Alien {

public:

	JsonCreate();
	virtual ~JsonCreate();
	
	void Start();
	void Update();

};

ALIEN_FACTORY JsonCreate* CreateJsonCreate() {
	JsonCreate* alien = new JsonCreate();
	// To show in inspector here

	return alien;
} 
