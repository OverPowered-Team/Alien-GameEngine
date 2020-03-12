#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#/*define TO_ENUM(ENUM, ...) enum ENUM {\
	__VA_ARGS__\
	};

#define enum(ENUM, ...) enum ENUM {\
	__VA_ARGS__\
	};


enum(MYENUM, one, two, three)

enum aaa {

};*/

//#define enumString(NAME, ...) static const char* NAME##_Names[] = { #__VA_ARGS__ };

enum(MYENUM, one, two, three)

class ALIEN_ENGINE_API TestEvents : public Alien {

public:

	TestEvents();
	virtual ~TestEvents();
	
	void Start();
	void Update();

	void Event();
	void Event2();
	void Event3();
};

ALIEN_FACTORY TestEvents* CreateTestEvents() {
	TestEvents* events = new TestEvents();
	// To show in inspector here
	SHOW_VOID_FUNCTION(TestEvents::Event, events);
	SHOW_VOID_FUNCTION(TestEvents::Event2, events);
	SHOW_VOID_FUNCTION(TestEvents::Event3, events);
	for (uint i = 0; i < 3; ++i) {
		LOG(MYENUMEnumNames);
	}
	return events;
} 
