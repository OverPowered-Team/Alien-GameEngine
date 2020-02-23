#pragma once

#include "Module.h"
#include "Globals.h"
#include "WwiseT.h"

#include <list>

class ComponentAudioEmitter;

struct BankEvent {
	u64 id;
	std::string name;
};

struct AudioFiles {
	u64 id;
	std::string name;
};

struct Bank {
	u64 id;
	std::string name;
	std::vector<BankEvent> events;
	std::vector <AudioFiles> audios;
};


class ModuleAudio : public Module
{
public:
	ModuleAudio(bool start_enabled = true);
	~ModuleAudio();
	bool Start();
	void LoadBanksInfo();
	void LoadUsedBanks();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	bool UnloadAllBanks();
	void UnloadAllUsedBanks();
	// Utils
	void Play(const char* event);
	void Play();
	void Stop();
	void Pause() const;
	void Resume() const;
	void SetListener(WwiseT::AudioSource* new_listener);
	WwiseT::AudioSource* CreateSoundEmitter(const char * name);
	std::vector<Bank> GetBanks();
	Bank GetBankByName(const char* name);
private:
	WwiseT::AudioSource* listener;
	std::vector <Bank> banks;
	bool play_mode = false;
public:
	std::list<ComponentAudioEmitter*> emitters;
	std::list<Bank> used_banks;
	bool is_playing = false;
};
