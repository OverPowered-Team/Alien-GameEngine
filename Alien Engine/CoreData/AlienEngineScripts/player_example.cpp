#include "player_example.h"
#include "..\..\ComponentBar.h"

player_example::player_example() : Alien()
{
}

player_example::~player_example()
{
}

void player_example::OnDrawGizmos()
{
	
}

void player_example::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_H)) {
		GameObject::Instantiate("Cube", float3::zero());
	}
}