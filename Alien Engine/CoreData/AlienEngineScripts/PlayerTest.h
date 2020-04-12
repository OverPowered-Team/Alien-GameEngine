#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum(MYENEUM, dsada, asdadsa, dsadsad, saddd);

class ALIEN_ENGINE_API PlayerTest : public Alien {

public:

	PlayerTest();
	virtual ~PlayerTest();
	
	void Start();

	std::string myString;

	MYENEUM enumm = MYENEUM::dsada;
};

ALIEN_FACTORY PlayerTest* CreatePlayerTest() {
	PlayerTest* alien = new PlayerTest();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_STRING(alien->myString);
	SHOW_IN_INSPECTOR_AS_ENUM(MYENEUM, alien->enumm);
	return alien;
} 
