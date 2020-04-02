#include "Instatiate.h"

Instatiate::Instatiate() : Alien()
{
}

Instatiate::~Instatiate()
{
}

void Instatiate::Start()
{
}

void Instatiate::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_I))
	{
		GameObject::Instantiate(prefab, float3(20, 20, 20), true);
	}
}