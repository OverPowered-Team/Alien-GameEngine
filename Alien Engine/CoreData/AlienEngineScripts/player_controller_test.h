#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API player_controller_test : public Alien {

public:

	player_controller_test();
	virtual ~player_controller_test();
	
	void Start();
	void Update();

	void OnControllerColliderHit(const ControllerColliderHit& hit);

public:
	ComponentCharacterController* ccontroller = nullptr;

	float speed = 10.0f;
	float jumpSpeed = 10.0f;
	float gravity = 20.0f;

	float3 moveDirection;

};

ALIEN_FACTORY player_controller_test* Createplayer_controller_test() {
	player_controller_test* alien = new player_controller_test();
	// To show in inspector here
	//SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->ccontroller->velocity);

	return alien;
} 
