#include "TestTweenLineal.h"

TestTweenLineal::TestTweenLineal() : Alien()
{
}

TestTweenLineal::~TestTweenLineal()
{
}

void TestTweenLineal::Start()
{
}

void TestTweenLineal::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_A))
		Tween::CreateTween(this->game_object, float4(30, 0, 0, 0), 5, Tween::MOVE, Tween::linear);
}