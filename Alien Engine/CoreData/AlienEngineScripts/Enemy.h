#pragma once

#include "..\..\Alien.h"
#include "Macros/AlienScripts.h"

class Enemy : public Alien {

public: 

	struct EnemyStats {
		float health = 0.0F;
		float agility = 0.0F;
		float damage = 0.0F;
	};

	enum (EnemyType,
		NONE = -1,
		RANGED,
		NILFGAARD_SOLDIER
	);

	enum (EnemyState,
		NONE = -1,
		IDLE,
		MOVE,
		ATTACK,
		BLOCK,
		DEAD,
		FLEE,
		);

public:

	Enemy();
	virtual ~Enemy();

	virtual void Start();
	virtual void SetStats() {}
	virtual void Update() {}
	virtual void CleanUp() {}

public:
		
	EnemyType type = EnemyType::NONE;
	EnemyStats stats;
	EnemyState state = EnemyState::NONE;
	ComponentAnimator* animator = nullptr;
};