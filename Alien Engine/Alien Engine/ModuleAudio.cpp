#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"

ModuleAudio::ModuleAudio()
{}

ModuleAudio::~ModuleAudio()
{}

bool ModuleAudio::Start()
{
	// Init wwise and audio banks
	WwiseT::InitSoundEngine();
	WwiseT::LoadBank("Assignment3.bnk");
	
	//source = App->audio->CreateSoundEmitter("Emitter");
	//SetListener(source);

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
	return true;
}

WwiseT::AudioSource * ModuleAudio::CreateSoundEmitter(const char * name)
{
	WwiseT::AudioSource* ret = WwiseT::CreateAudSource(name);
	return ret;
}

void ModuleAudio::Play()
{
	
	std::list<AudioEmitter*>::const_iterator iterator;
	for (iterator = audios.begin(); iterator != App->audio->audios.end(); ++iterator)
	{	
			iterator._Ptr->_Myval->StartSound();
	}

	is_playing = true;
}

void ModuleAudio::Stop()
{
	std::list<AudioEmitter*>::const_iterator iterator;
	for (iterator = audios.begin(); iterator != App->audio->audios.end(); ++iterator)
	{

		iterator._Ptr->_Myval->source->StopEventByName("BGmusic");
		iterator._Ptr->_Myval->source->StopEventByName("Rain");

	}

	is_playing = false;
}

void ModuleAudio::Pause() const
{
	std::list<AudioEmitter*>::const_iterator iterator;
	for (iterator = audios.begin(); iterator != App->audio->audios.end(); ++iterator)
	{

		iterator._Ptr->_Myval->source->PauseEventByName("BGmusic");
		iterator._Ptr->_Myval->source->PauseEventByName("Rain");

	}
}

void ModuleAudio::Resume() const
{
	std::list<AudioEmitter*>::const_iterator iterator;
	for (iterator = audios.begin(); iterator != App->audio->audios.end(); ++iterator)
	{

		iterator._Ptr->_Myval->source->ResumeEventByName("BGmusic");
		iterator._Ptr->_Myval->source->ResumeEventByName("Rain");

	}
}

void ModuleAudio::SetListener(WwiseT::AudioSource* new_listener)
{
	listener = new_listener;
	WwiseT::SetDefaultListener(new_listener->GetID());
}

