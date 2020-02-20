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
	// Init wwise and audio banks
	WwiseT::InitSoundEngine();
	WwiseT::LoadBank("Main.bnk");

	auto s = WwiseT::CreateAudSource("hola");

	s->PlayEventByName("test");

	WwiseT::SetDefaultListener(s->GetID());

	return true;
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
	audios.clear();
	WwiseT::StopAllEvents();
	return WwiseT::CloseSoundEngine();
}

WwiseT::AudioSource * ModuleAudio::CreateSoundEmitter(const char * name)
{
	WwiseT::AudioSource* ret = WwiseT::CreateAudSource(name);
	return ret;
}

void ModuleAudio::Play()
{
	for (auto iterator = audios.begin(); iterator != App->audio->audios.end(); ++iterator)
	{
		(*iterator)->StartSound();
	}

	is_playing = true;
}

void ModuleAudio::Stop()
{
	for (auto iterator = audios.begin(); iterator != App->audio->audios.end(); ++iterator)
	{
		(*iterator)->source->StopEventByName("BGmusic");
		(*iterator)->source->StopEventByName("Rain");
	}

	is_playing = false;
}

void ModuleAudio::Pause() const
{
	for (auto iterator = audios.begin(); iterator != App->audio->audios.end(); ++iterator)
	{
		(*iterator)->source->PauseEventByName("BGmusic");
		(*iterator)->source->PauseEventByName("Rain");
	}
}

void ModuleAudio::Resume() const
{
	for (auto iterator = audios.begin(); iterator != App->audio->audios.end(); ++iterator)
	{
		(*iterator)->source->ResumeEventByName("BGmusic");
		(*iterator)->source->ResumeEventByName("Rain");
	}
}

void ModuleAudio::SetListener(WwiseT::AudioSource* new_listener)
{
	listener = new_listener;
	WwiseT::SetDefaultListener(new_listener->GetID());
}

