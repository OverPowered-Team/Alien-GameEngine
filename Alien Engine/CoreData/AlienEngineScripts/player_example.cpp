#include "player_example.h"

player_example::player_example() : Alien()
{
}

player_example::~player_example()
{
}



void player_example::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_3)) {
		GameObject::Instantiate(p, { 0,0,0 }, false, GameObject::FindWithName("Canvas"));
	}
}