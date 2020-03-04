#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API asd : public Alien {

public:

	asd();
	virtual ~asd();
	
	void Start();
	void Update();

};

ALIEN_FACTORY asd* Createasd() {
	asd* alien = new asd();
	// To show in inspector here

	return alien;
} 
