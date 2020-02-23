#pragma once

#include "Component.h"
#include "Globals.h"
#include "WwiseT.h"
#include "Timer.h"

#include <string>

struct Bank;

class __declspec(dllexport) ComponentAudioEmitter : public Component
{
public:
	ComponentAudioEmitter(GameObject* parent);
	~ComponentAudioEmitter();
public:

	void Update() override;
	void ChangeVolume(float new_volume);
	void Mute(bool mute);
	void StartSound();
	void UpdateSourcePos();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	bool DrawInspector();
	bool AlreadyUsedBank(Bank bk);
public:
	WwiseT::AudioSource* source = nullptr;
	std::string audio_name;
	std::string current_bank;
	std::string current_event;
	float volume = 1.0f;
	bool play_on_awake = true;
	bool loop = false;
	bool mute = false;

private:
	bool play_mode = false;
};
