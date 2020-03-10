#include "TestEvents.h"

TestEvents::TestEvents() : Alien()
{
}

TestEvents::~TestEvents()
{
}

void TestEvents::Start()
{
}

void TestEvents::Update()
{
}

void TestEvents::Event()
{
	LOG("FUNCTION EVENT CALLED");
}

void TestEvents::Event2()
{
	LOG("FUNCTION EVENT2 CALLED");
}

void TestEvents::Event3()
{
	LOG("FUNCTION EVENT3 CALLED");
}

void TestEvents::GoToScene(int sceneNum, const char* name)
{
	int num = sceneNum;
}
