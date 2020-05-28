#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RelicManager : public Alien {

public:

	RelicManager();
	virtual ~RelicManager();

	void Start();
	void Update();

	void DropRelic(float3 position);

	Prefab fire_runestone;
	Prefab ice_runestone;
	Prefab earth_runestone;
	Prefab lightning_runestone;
	Prefab poison_runestone;

	Prefab fire_oil;
	Prefab ice_oil;
	Prefab earth_oil;
	Prefab poison_oil;

	GameObject* spawn = nullptr;

};

ALIEN_FACTORY RelicManager* CreateRelicManager() {
	RelicManager* relic = new RelicManager();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(relic->fire_runestone);
	SHOW_IN_INSPECTOR_AS_PREFAB(relic->ice_runestone);
	SHOW_IN_INSPECTOR_AS_PREFAB(relic->earth_runestone);
	SHOW_IN_INSPECTOR_AS_PREFAB(relic->lightning_runestone);
	SHOW_IN_INSPECTOR_AS_PREFAB(relic->poison_runestone);

	SHOW_IN_INSPECTOR_AS_PREFAB(relic->fire_oil);
	SHOW_IN_INSPECTOR_AS_PREFAB(relic->ice_oil);
	SHOW_IN_INSPECTOR_AS_PREFAB(relic->earth_oil);
	SHOW_IN_INSPECTOR_AS_PREFAB(relic->poison_oil);

	return relic;
}