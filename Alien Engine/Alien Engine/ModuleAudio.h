#pragma once

#include "Module.h"
#include "Globals.h"
#include "WwiseT.h"

#include <list>

class ComponentAudioEmitter;

struct Bank {
	u64 id = 0ULL;
	std::string name = "";
	std::map<u64, std::string> events;
	std::map<u64, std::string> audios;
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
	bool UnloadAllBanksFromWwise();
	void UnloadAllUsedBanksFromWwise();
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
	Bank GetBankByID(const u64& id);
private:
	WwiseT::AudioSource* listener;
	std::vector <Bank> banks;
	bool play_mode = false;

public:
	std::list<ComponentAudioEmitter*> emitters;
	std::list<Bank> used_banks;
	bool is_playing = false;
};
