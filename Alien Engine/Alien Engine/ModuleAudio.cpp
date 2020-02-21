#include "Application.h"
#include "ModuleAudio.h"
#include "ComponentAudioEmitter.h"

ModuleAudio::ModuleAudio(bool start_enabled) : Module(start_enabled)
{
	name = "audio";
}

ModuleAudio::~ModuleAudio()
{}

bool ModuleAudio::Start()
{
	bool ret = true;

	// Init wwise and audio banks
	ret = WwiseT::InitSoundEngine();
	WwiseT::LoadBank("Main.bnk");

	return ret;
}

update_status ModuleAudio::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleAudio::PostUpdate(float dt)
{
	WwiseT::ProcessAudio();
	return UPDATE_CONTINUE;
}

bool ModuleAudio::CleanUp()
{
	for (auto i = emitters.begin(); i != emitters.end(); i++)
		delete* i;
	emitters.clear();

	WwiseT::StopAllEvents();
	UnloadAllBanks();
	return WwiseT::CloseSoundEngine();
}

bool ModuleAudio::UnloadAllBanks()
{
	for (auto it = eng_banks.begin(); it != eng_banks.end(); it++)
	{
		WwiseT::UnLoadBank((*it).c_str());
	}
	eng_banks.clear();

	return true;
}

WwiseT::AudioSource * ModuleAudio::CreateSoundEmitter(const char * name)
{
	return WwiseT::CreateAudSource(name);
}

void ModuleAudio::Play()
{
	for (auto iterator = emitters.begin(); iterator != App->audio->emitters.end(); ++iterator)
	{
		(*iterator)->StartSound();
	}

	is_playing = true;
}

void ModuleAudio::Stop()
{
	WwiseT::StopAllEvents();
}

void ModuleAudio::Pause() const
{
	WwiseT::PauseAll();
}

void ModuleAudio::Resume() const
{
	WwiseT::ResumeAll();
}

void ModuleAudio::SetListener(WwiseT::AudioSource* new_listener)
{
	listener = new_listener;
	WwiseT::SetDefaultListener(new_listener->GetID());
}

