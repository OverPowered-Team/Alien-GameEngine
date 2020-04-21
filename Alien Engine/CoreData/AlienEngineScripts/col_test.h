#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API col_test : public Alien {

public:

	col_test();
	virtual ~col_test();
	
	void Start();
	void Update();

	void OnCollisionEnter(const Collision& collision);
};

ALIEN_FACTORY col_test* Createcol_test() {
	col_test* alien = new col_test();
	// To show in inspector here

	return alien;
} 
