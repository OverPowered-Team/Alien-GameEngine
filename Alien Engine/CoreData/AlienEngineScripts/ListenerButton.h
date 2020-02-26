#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#define ListenerButton ListenerButton
#define TOstring(name) #name
#define LISTENERBUTTONFACTORY Create + TOstring(ListenerButton)

class ALIEN_ENGINE_API ListenerButton : public Alien {

	CREATESELF(ListenerButton)

public:

	ListenerButton();
	virtual ~ListenerButton();
	
	void Start();
	void Update();

	void Exit();

	void NeFunct();
};

ALIEN_FACTORY ListenerButton* CreateListenerButton() {
	ListenerButton* alien = new ListenerButton();
	// To show in inspector here

	return alien;
} 
