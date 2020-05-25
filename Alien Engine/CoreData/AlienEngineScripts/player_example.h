#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API player_example : public Alien {

public:

	player_example();
	virtual ~player_example();

	void Update();

public:
	Prefab p;
};


ALIEN_FACTORY player_example* Createplayer_example() {
	player_example* alien = new player_example();
	// To show in inspector here


	SHOW_IN_INSPECTOR_AS_PREFAB(alien->p);
	return alien;
} 
