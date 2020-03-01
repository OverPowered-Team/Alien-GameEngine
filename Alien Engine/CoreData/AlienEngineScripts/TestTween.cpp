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
		Tween::TweenRGBA(this->game_object, float4(0, 1.0f, 0.5, 0.5), 5);
}