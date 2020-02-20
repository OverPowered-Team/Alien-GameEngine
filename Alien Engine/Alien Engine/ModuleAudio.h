#pragma once

#include "Module.h"
#include "Globals.h"
#include "WwiseT.h"

#include <list>

class ComponentAudioEmitter;

class ModuleAudio : public Module
{
public:
	ModuleAudio(bool start_enabled = true);
	~ModuleAudio();
	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	bool UnloadAllBanks();

	// Utils
	void Play();
	void Stop();
	void Pause() const;
	void Resume() const;
	void SetListener(WwiseT::AudioSource* new_listener);
	WwiseT::AudioSource* CreateSoundEmitter(const char * name);

private:
	WwiseT::AudioSource* listener;
	std::vector <std::string> eng_banks;

public:
	std::list<ComponentAudioEmitter*> emitters;
	
	bool is_playing = false;
};
