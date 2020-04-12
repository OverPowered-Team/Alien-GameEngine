#include "PlayerTest.h"

PlayerTest::PlayerTest() : Alien()
{
}

PlayerTest::~PlayerTest()
{
}

void PlayerTest::Start()
{
	ComponentCamera* camera = GetComponent<ComponentCamera>();
	std::vector<ComponentCamera*> cameras = GetComponents<ComponentCamera>();
	LOG("%s", camera->game_object_attached->GetName());
	LOG("%s", cameras.back()->game_object_attached->GetTag());
	int i = 0;

	PlayerTest* test = GetComponent<PlayerTest>();
	LOG("%s", test->ToString());

}
