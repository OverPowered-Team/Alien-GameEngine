#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API trigTest : public Alien {

public:

	trigTest();
	virtual ~trigTest();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* col);

};

ALIEN_FACTORY trigTest* CreatetrigTest() {
	trigTest* alien = new trigTest();
	// To show in inspector here

	return alien;
} 
