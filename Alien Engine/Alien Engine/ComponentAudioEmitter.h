#pragma once

#include "Component.h"
#include "Globals.h"
#include "WwiseT.h"
#include "Timer.h"

#include <string>

struct Bank;

class __declspec(dllexport) ComponentAudioEmitter : public Component
{
	friend class GameObject;
public:
	ComponentAudioEmitter(GameObject* parent);
	~ComponentAudioEmitter();

//script
public:
	void ChangeVolume(float new_volume);
	void Mute(bool mute);
	void StartSound();
	void StartSound(uint _event);
	bool AlreadyUsedBank(const Bank* bk);
	u64 GetCurrentBank();

private:
	void UpdateSourcePos();
	void Update() override;
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	void OnEnable() override;
	void OnDisable() override;
	bool DrawInspector();
	

private:
	WwiseT::AudioSource* source = nullptr;
	std::string audio_name;
	u64 current_bank = 0ull;
	u64 current_event = 0ull;
	float volume = 1.0f;
	bool play_on_awake = true;
	bool loop = false;
	bool mute = false;
	bool play_mode = false;

};
