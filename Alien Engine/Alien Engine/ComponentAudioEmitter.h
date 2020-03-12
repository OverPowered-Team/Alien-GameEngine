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
	u64 GetCurrentBank();
	u32 GetWwiseIDFromString(const char* Wwise_name) const;
	void SetSwitchState(u32 switch_group_id, u32 switch_state_id);
	void SetReverb(const float& strength, const char* name);
	WwiseT::AudioSource* GetSource() const;
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
