#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum(AAAAAAA,
	dsadsad,
	sdasd,
	sdasdsad);

class ALIEN_ENGINE_API PolScriptSushi : public Alien {

public:

	PolScriptSushi();
	virtual ~PolScriptSushi();
	
	void Start();
	void Update();

	String myString;

	AAAAAAA dsadsa = AAAAAAA::sdasd;
};

ALIEN_FACTORY PolScriptSushi* CreatePolScriptSushi() {
	PolScriptSushi* sushi = new PolScriptSushi();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_STRING(sushi->myString);
	SHOW_IN_INSPECTOR_AS_ENUM(AAAAAAA, sushi->dsadsa);
	return sushi;
} 
