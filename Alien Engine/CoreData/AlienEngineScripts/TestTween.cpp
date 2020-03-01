#include "TestTween.h"

TestTween::TestTween() : Alien()
{
}

TestTween::~TestTween()
{
}

void TestTween::Start()
{
}

void TestTween::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_A))
		Tween::CreateTween(this->game_object, float4(1.0f, 0, 0, 0), 10, Tween::COLOR, Tween::linear);
}