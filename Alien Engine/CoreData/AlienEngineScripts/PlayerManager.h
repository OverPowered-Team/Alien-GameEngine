#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;
class InGame_UI;
class UltiBar;

class ALIEN_ENGINE_API PlayerManager : public Alien {

public:

	PlayerManager();
	virtual ~PlayerManager();
	
	void Start();
	void Update();

	void OnPlayerDead(PlayerController* dead_player);
	void OnPlayerRevive(PlayerController* revived_player);
	void BlockInput(bool block);

	int max_ultimate_charge = 200;
	float ultimate_time = 5.0f;
	float ultimate_effect_value = 0.5f;

	int collective_ultimate_charge = 0;
	uint ultimate_buttons_pressed = 0;
	bool ultimate_is_active = false;

	std::vector<PlayerController*> players;
	std::vector<PlayerController*> players_dead;

	void IncreaseUltimateCharge(uint value);

private:
	void ActivateUltimate();
	void CancelUltimate();

private:

	// UI ultibar
	InGame_UI* in_game_ui = nullptr;
	UltiBar* ultibar = nullptr;

};

ALIEN_FACTORY PlayerManager* CreatePlayerManager() {
	PlayerManager* alien = new PlayerManager();

	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->max_ultimate_charge);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->ultimate_time);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->ultimate_effect_value);

	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->collective_ultimate_charge);

	return alien;
} 
