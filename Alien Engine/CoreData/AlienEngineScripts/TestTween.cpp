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
		Tween::TweenRotate(this->game_object, float3(90.f, 0.0f, 0.F), 5);
}