#include "DebugManager.h"
#include "PlayerController.h"
#include "UI_Char_Frame.h"

DebugManager::DebugManager() : Alien()
{
}

DebugManager::~DebugManager()
{
}

void DebugManager::Start()
{
	geralt_controller = GameObject::FindWithName("Geralt_Prefab")->GetComponent<PlayerController>();
	yennefer_controller = GameObject::FindWithName("Yennefer")->GetComponent<PlayerController>();
	main_camera = Camera::GetCurrentCamera();
}

void DebugManager::Update()
{
	if (Input::GetKeyRepeat(SDL_SCANCODE_F3))
	{
		if (Input::GetKeyDown(SDL_SCANCODE_F1))
		{
			geralt_controller->player_data.stats["Health"].IncreaseStat(geralt_controller->player_data.stats["Health"].GetMaxValue());
			yennefer_controller->player_data.stats["Health"].IncreaseStat(yennefer_controller->player_data.stats["Health"].GetMaxValue());
			geralt_controller->HUD->GetComponent<UI_Char_Frame>()->LifeChange(geralt_controller->player_data.stats["Health"].GetMaxValue(), geralt_controller->player_data.stats["Health"].GetMaxValue());
			yennefer_controller->HUD->GetComponent<UI_Char_Frame>()->LifeChange(yennefer_controller->player_data.stats["Health"].GetMaxValue(), yennefer_controller->player_data.stats["Health"].GetMaxValue());
		}
		if (Input::GetKeyDown(SDL_SCANCODE_0))
		{
			SceneManager::LoadScene("Main_Menu");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_1))
		{
			SceneManager::LoadScene("Lvl_1");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_2))
		{
			SceneManager::LoadScene("Lvl_1_Tutorial");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_3))
		{
			SceneManager::LoadScene("Level_Mahakam");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_4))
		{
			SceneManager::LoadScene("Wagonnetes");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_5))
		{
			SceneManager::LoadScene("Level_Mahakam_Deepnest");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_6))
		{
			SceneManager::LoadScene("Level_Mahakam_Boss");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_W))
		{
			SceneManager::LoadScene("NewWin_Menu");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_D))
		{
			SceneManager::LoadScene("EndGame_Menu");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_F))
		{
			if (Camera::GetCurrentCamera() == main_camera)
			{
				ComponentCamera* aux_cam = GameObject::FindWithName("Aux Camera")->GetComponent<ComponentCamera>();
				if (aux_cam)
					Camera::SetCurrentCamera(aux_cam);
			}
			else
				Camera::SetCurrentCamera(main_camera);

		}

		if (Input::GetKeyDown(SDL_SCANCODE_G))
		{
			geralt_controller->godmode = !geralt_controller->godmode;
			yennefer_controller->godmode = !yennefer_controller->godmode;
			GameObject::FindWithName("Godmode")->SetEnable(!GameObject::FindWithName("Godmode")->IsEnabled());
		}
	}

}