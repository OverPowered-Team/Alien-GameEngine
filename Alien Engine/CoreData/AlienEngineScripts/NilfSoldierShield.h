#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "NilfgaardSoldier.h"

class ALIEN_ENGINE_API NilfSoldierShield : public NilfgaardSoldier {
public:

	NilfSoldierShield();
	virtual ~NilfSoldierShield();
	
	void UpdateEnemy() override;
	void Action() override;
	void Block();
	bool CheckPlayerForward();

	float GetDamaged(float dmg, PlayerController* player, float3 knock_back) override;

public:
	float block_time = 2.0f;
	float block_attack_time = 3.0f;
	int max_break_shield_attack = 5;

private:
	float current_time = 0.0f;
	bool has_been_attacked = false;
	bool is_blocked = false;
	int break_shield_attack = 0;

};

ALIEN_FACTORY NilfSoldierShield* CreateNilfSoldierShield() {
	NilfSoldierShield* nilfgaard = new NilfSoldierShield();

	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(nilfgaard->increase_hit_animation);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(nilfgaard->gravity);
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldierState, nilfgaard->state);
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldier::NilfgaardType, nilfgaard->nilf_type);
	SHOW_IN_INSPECTOR_AS_PREFAB(nilfgaard->head_prefab);
	SHOW_VOID_FUNCTION(Enemy::SpawnAttackParticle, nilfgaard);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::ActivateCollider, nilfgaard);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::DeactivateCollider, nilfgaard);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(nilfgaard->block_time);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(nilfgaard->block_attack_time);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_INT(nilfgaard->max_break_shield_attack);
	return nilfgaard;
} 
