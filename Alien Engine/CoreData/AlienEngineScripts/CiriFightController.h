#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#include "CiriOriginal.h"
#include "Ciri.h"

class ALIEN_ENGINE_API CiriFightController : public Alien {
public:
	bool fight_started = false;
	float ciri_clones_scream_cd = 15.0f;
	float scream_cd_timer = 0.0f;
	bool can_mini_scream = true;
	float phase_0_timer = 0.0f;
	float phase_0_time = 3.0f;
	float phase_4_timer = 0.0f;
	float phase_4_time = 3.0f;
	bool phase_change = false;

	int clones_dead = 0;

	int phase = 0;

	std::vector<GameObject*> clone_positions;
public:

	CiriFightController();
	virtual ~CiriFightController();
	
	void Start();
	void Update();

	void UpdatePhaseZero();
	void FinishPhaseZero();
	void UpdatePhaseOne();
	void FinishPhaseOne();
	void UpdatePhaseTwo();
	void FinishPhaseTwo();
	void UpdatePhaseThree();
	void FinishPhaseThree();
	void FinishPhaseFour();
	void UpdatePhaseFour();

	void ChangeToPhase2();
	void ChangeToPhase3();
	void ChangeToPhase4();

	void OnCloneDead(GameObject* clone);

};

ALIEN_FACTORY CiriFightController* CreateCiriFightController() {

	CiriFightController* cirifightcontroller = new CiriFightController();
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(cirifightcontroller->ciri_clones_scream_cd);

	return cirifightcontroller;
} 
