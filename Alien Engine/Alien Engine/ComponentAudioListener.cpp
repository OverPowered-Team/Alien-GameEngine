#include "ComponentAudioListener.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "MathGeoLib\include\MathGeoLib.h"

AudioListener::AudioListener(GameObject * parent) : Component(parent)
{
	type = ComponentType::A_LISTENER;

	listener = App->audio->CreateSoundEmitter("listener");
	App->audio->SetListener(listener);
}

AudioListener::~AudioListener()
{
	//RELEASE(listener);
}

void AudioListener::Update(float dt)
{

}

void AudioListener::UpdateListenerPos()
{
	
}

void AudioListener::LoadComponent(JSONArraypack* to_load)
{
	to_load->SetNumber("Type", (int)type);
}
