#include "TestSwitches.h"

TestSwitches::TestSwitches() : Alien()
{
}

TestSwitches::~TestSwitches()
{
}

void TestSwitches::Start()
{
	emmitter = (ComponentAudioEmitter*)GetComponent(ComponentType::A_EMITTER);
}

void TestSwitches::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_U))
	{
		emmitter->StartSound();
	}
	if (Input::GetKeyDown(SDL_SCANCODE_T))
	{
		int group_id = emmitter->GetWwiseIDFromString("Material");
		int state_id = emmitter->GetWwiseIDFromString("Dirt");
		emmitter->SetSwitchState(group_id, state_id);
	}
	if (Input::GetKeyDown(SDL_SCANCODE_Y))
	{
		int group_id = emmitter->GetWwiseIDFromString("Material");
		int state_id = emmitter->GetWwiseIDFromString("Sand");
		emmitter->SetSwitchState(group_id, state_id);
	}
}
