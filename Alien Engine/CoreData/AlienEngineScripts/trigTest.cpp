#include "trigTest.h"

trigTest::trigTest() : Alien()
{
}

trigTest::~trigTest()
{
}

void trigTest::Start()
{
}

void trigTest::Update()
{
}

void trigTest::OnTriggerEnter(ComponentCollider* col)
{
	LOG_ENGINE("trigger test col %s", col->game_object_attached->GetName());
}