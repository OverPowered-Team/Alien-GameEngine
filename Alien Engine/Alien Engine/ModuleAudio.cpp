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
	//WwiseT::LoadBank("Main.bnk");
	//PrepareAllBanks();
	LoadBanksInfo();
	return ret;
}

void ModuleAudio::LoadBanksInfo()
{
	auto j = App->LoadJSONFile("DLLs/SoundbanksInfo.json");
	auto bank_arr = j->GetArray("SoundBanksInfo.SoundBanks");
	bank_arr->GetFirstNode();
	for (uint i = 0; i < bank_arr->GetArraySize(); ++i, bank_arr->GetAnotherNode()) {
		if (strcmp(bank_arr->GetString("ShortName"), "Init") != 0) {
			Bank b;
			b.id = std::stoull(bank_arr->GetString("Id"));
			b.name = bank_arr->GetString("ShortName");
			auto events = bank_arr->GetArray("IncludedEvents");
			events->GetFirstNode();
			for (uint e = 0; e < events->GetArraySize(); ++e, events->GetAnotherNode()) {
				BankEvent ev = BankEvent();
				ev.id = std::stoull(events->GetString("Id"));
				ev.name = events->GetString("Name");
				b.events.push_back(ev);
			}
			auto aud = bank_arr->GetArray("IncludedMemoryFiles");
			for (uint a = 0; a < aud->GetArraySize(); ++a) {
				AudioFiles f;
				f.id = std::stoull(aud->GetString("Id"));
				f.name = aud->GetString("ShortName");
				b.audios.push_back(f);
			}
			banks.push_back(b);
		}
	}
}

update_status ModuleAudio::Update(float dt)
{
	if (Time::state == Time::GameState::NONE) {
		if (play_mode) {
			UnloadAllUsedBanks();
			play_mode = false;
		}
	}
	else if (Time::state == Time::GameState::PLAY) {
		if (!play_mode) {
			LoadUsedBanks();
			//WwiseT::LoadBank("Main.bnk");
			play_mode = true;
		}
	}
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

void ModuleAudio::LoadUsedBanks()
{
	for (auto i = App->audio->used_banks.begin(); i != App->audio->used_banks.end(); i++)
	{
		std::string full_path = (*i).name + ".bnk";
		WwiseT::LoadBank(full_path.c_str());
	}
}


bool ModuleAudio::UnloadAllBanks()
{
	for (auto it = banks.begin(); it != banks.end(); it++)
	{
		WwiseT::UnLoadBank((*it).name.c_str());
	}
	banks.clear();

	return true;
}

void ModuleAudio::UnloadAllUsedBanks()
{
	for (auto it = used_banks.begin(); it != used_banks.end(); it++)
	{
		WwiseT::UnLoadBank((*it).name.c_str());
	}
	used_banks.clear();

}

WwiseT::AudioSource * ModuleAudio::CreateSoundEmitter(const char * name)
{
	return WwiseT::CreateAudSource(name);
}

std::vector<Bank> ModuleAudio::GetBanks()
{
	return banks;
}

Bank ModuleAudio::GetBankByName(const char* name)
{
	Bank bk;
	for (int i = 0; i < banks.size(); ++i)
	{
		if (App->StringCmp(name, App->audio->GetBanks()[i].name.c_str()))
			bk = App->audio->GetBanks()[i];
	}
	return bk;
}

void ModuleAudio::Play(const char* event)
{
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

