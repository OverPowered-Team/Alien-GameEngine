#include "player_example.h"
#include "..\..\ComponentBar.h"

player_example::player_example() : Alien()
{
}

player_example::~player_example()
{
}

void player_example::Update()
{
	if (Input::GetKeyRepeat(SDL_SCANCODE_3)) {
		ComponentBar* bar = GetComponent<ComponentBar>();
		float value = bar->GetBarValue();
		value += Time::GetDT();
		if (value > 100) {
			value = 100;
		}
		bar->SetBarValue(value);
	}
	if (Input::GetKeyRepeat(SDL_SCANCODE_4)) {
		ComponentBar* bar = GetComponent<ComponentBar>();
		float value = bar->GetBarValue();
		value -= Time::GetDT();
		if (value < 0) {
			value = 0;
		}
		bar->SetBarValue(value);
	}
}