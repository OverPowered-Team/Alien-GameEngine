#include "StaticInput.h"
#include "Application.h"
#include "PanelGame.h"

bool Input::GetKeyDown(const SDL_Scancode& code)
{
	return IsInputAvailable() && App->input->GetKey(code) == KEY_DOWN;
}

bool Input::GetKeyIdle(const SDL_Scancode& code)
{
	return IsInputAvailable() && App->input->GetKey(code) == KEY_IDLE;
}

bool Input::GetKeyUp(const SDL_Scancode& code)
{
	return IsInputAvailable() && App->input->GetKey(code) == KEY_UP;
}

bool Input::GetKeyRepeat(const SDL_Scancode& code)
{
	return IsInputAvailable() && App->input->GetKey(code) == KEY_REPEAT;
}

SDL_Scancode Input::GetFirstKeyDown()
{
	if (IsInputAvailable()) {
		return App->input->GetFirstKeyPressed();
	}
	else {
		return SDL_SCANCODE_UNKNOWN;
	}
}

float3 Input::GetMousePosition()
{
#ifndef GAME_VERSION
	return float3((App->input->GetMouseX() - App->ui->panel_game->posX), (App->input->GetMouseY() - App->ui->panel_game->posY) - 19, App->input->GetMouseZ());
#else
	return App->input->GetMousePosition();
#endif 
}

float Input::GetMouseY()
{
#ifndef GAME_VERSION
	return App->input->GetMouseY() - App->ui->panel_game->posY - 19;
#else
	return App->input->GetMouseY();
#endif
}

float Input::GetMouseX()
{
#ifndef GAME_VERSION
	return App->input->GetMouseX() - App->ui->panel_game->posX;
#else
	return App->input->GetMouseX();
#endif 
}

bool Input::IsAnyMouseButtonPressed()
{
	return IsInputAvailable() && App->input->IsMousePressed();
}

float Input::GetMouseWheel()
{
	return App->input->GetMouseZ();
}

float Input::GetMouseYMotion()
{
	return App->input->GetMouseYMotion();
}

float Input::GetMouseXMotion()
{
	return App->input->GetMouseXMotion();
}

bool Input::GetMouseButtonDown(const MOUSE_BUTTONS& button)
{
	return IsInputAvailable() && App->input->GetMouseButton(button) == KEY_DOWN;
}

bool Input::GetMouseButtonUp(const MOUSE_BUTTONS& button)
{
	return IsInputAvailable() && App->input->GetMouseButton(button) == KEY_UP;
}

bool Input::GetMouseButtonIdle(const MOUSE_BUTTONS& button)
{
	return IsInputAvailable() && App->input->GetMouseButton(button) == KEY_IDLE;
}

bool Input::GetMouseButtonRepeat(const MOUSE_BUTTONS& button)
{
	return IsInputAvailable() && App->input->GetMouseButton(button) == KEY_REPEAT;
}

bool Input::IsInputAvailable()
{
#ifndef GAME_VERSION
	return App->ui->panel_game->game_focused;
#else
	return true;
#endif
}
