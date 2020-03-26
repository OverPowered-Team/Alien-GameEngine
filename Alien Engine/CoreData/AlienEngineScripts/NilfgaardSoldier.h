#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API NilfgaardSoldier : public Enemy {
public:

	NilfgaardSoldier() {}
	virtual ~NilfgaardSoldier() {}
	void SetStats() override;
	void Update() override;
	void CleanUp() override;

public:


};

ALIEN_FACTORY NilfgaardSoldier* CreateNilfgaardSoldier() {
	NilfgaardSoldier* alien = new NilfgaardSoldier();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Enemy::EnemyState, alien->state);
	return alien;
}

ALIEN_FACTORY void DestroyNilfgaardSoldier(void* pointer) {
	delete pointer;
	pointer = nullptr;
}


