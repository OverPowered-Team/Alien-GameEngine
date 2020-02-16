#pragma once

#include <string>

#include "Component.h"
#include "Globals.h"
#include "WwiseT.h"
#include "Timer.h"

class AudioEmitter : public Component
{
public:
	AudioEmitter(GameObject* parent);
	~AudioEmitter();

public:
	void Update(float dt);
	void ChangeVolume(float new_volume);
	void Mute(bool mute);
	void ChangeTimeToSwap(float new_time);
	void StartSound();
	void UpdateSourcePos();

	void SaveComponent(JSONArraypack* to_save) override;
	void LoadComponent(JSONArraypack* to_load) override;

public:
	WwiseT::AudioSource* source;
	std::string audio_name;
	float volume = 1.0f;
	float time_to_swap = 30.0f;
	int song = 2;
	bool mute = false;
	Timer timer;
};
