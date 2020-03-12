#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum (MYENUM, 
	one = -3, 
	two = 2, 
	three
);

class ALIEN_ENGINE_API TestEvents : public Alien {

public:

	TestEvents();
	virtual ~TestEvents();
	
	void Start();
	void Update();

	void Event();
	void Event2();
	void Event3();

public:

	MYENUM myEnumTest = MYENUM::two;
};

ALIEN_FACTORY TestEvents* CreateTestEvents() {
	TestEvents* events = new TestEvents();
	// To show in inspector here
	SHOW_VOID_FUNCTION(TestEvents::Event, events);
	SHOW_VOID_FUNCTION(TestEvents::Event2, events);
	SHOW_VOID_FUNCTION(TestEvents::Event3, events);

	SHOW_IN_INSPECTOR_AS_ENUM(MYENUM, events->myEnumTest);
	
	return events;
} 
