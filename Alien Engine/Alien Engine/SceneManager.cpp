#include "SceneManager.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "ResourceScene.h"
#include "ModuleFadeToBlack.h"

#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

int SceneManager::scenes_loaded = 0;

void SceneManager::LoadScene(const char* scene_name, FadeToBlackType ftb_type, float fade_seconds, float3 color)
{
	if (ftb_type == FadeToBlackType::NONE)
	{
		App->objects->sceneNameToChange.assign(scene_name);
	}
	else
	{
		App->fade_to_black->StartFade(fade_seconds, FadeType::FADE_TO, ftb_type, color,scene_name);

	}
}
static int TestThread(void* ptr)
{
	SDL_GL_MakeCurrent(App->window->window, App->renderer3D->thread_context);
	App->objects->LoadSceneParalel("minions");
	SDL_GL_MakeCurrent(App->window->window, NULL);
	//wglMakeCurrent(NULL, NULL);
	return 234567;
}
void SceneManager::LoadParalelScene(const char* scene_name)
{
	if (!App->objects->is_loading_scene_paralel) {
		//App->objects->paralel_thread = std::thread(std::bind(&ModuleObjects::LoadSceneParalel, App->objects, scene_name));
		SDL_Thread* thread;
		int         threadReturnValue;

		//wglMakeCurrent(NULL, NULL);
		/* Simply create a thread */
		thread = SDL_CreateThread(TestThread, "TestThread", (void*)NULL);

		if (NULL == thread) {
			printf("SDL_CreateThread failed: %s\n", SDL_GetError());
		}
		else {
			//SDL_WaitThread(thread, &threadReturnValue);
			//LOG_ENGINE("LAST");
			//SDL_GL_MakeCurrent(App->window->window, App->renderer3D->context);
		}

	}
}

int SceneManager::ScenesLoaded()
{
	return scenes_loaded;
}

const char* SceneManager::GetCurrentScene()
{
	return App->objects->current_scene->GetName();
}

void SceneManager::StartFade(float seconds, FadeType fade_type, FadeToBlackType ftb_type, float3 color)
{
	App->fade_to_black->StartFade(seconds, fade_type, ftb_type, color);
}
