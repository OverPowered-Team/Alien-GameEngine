#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API player_example : public Alien {

public:

	player_example();
	virtual ~player_example();

	void Start();
	void Update();

	// only performed when Move (controller) is called
	void OnControllerColliderHit(ControllerColliderHit hit);




	// COLLISION AND TRIGGERS
	void OnCollisionEnter(const Collision& collision);
	void OnCollisionStay(const Collision& collision);
	void OnCollisionExit(const Collision& collision);


	void OnTriggerEnter(ComponentCollider* collider);

	// etc

	void TEst(){}


public:
	ComponentCharacterController* ccontroller = nullptr;

	float speed = 10.0f;
	float jumpSpeed = 10.0f;
	float gravity = 20.0f;

	float3 moveDirection;

	GameObject* obj = nullptr;
};


ALIEN_FACTORY player_example* Createplayer_example() {
	player_example* alien = new player_example();
	// To show in inspector here
	SHOW_VOID_FUNCTION(player_example::TEst, alien);

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->obj);
	return alien;
} 
