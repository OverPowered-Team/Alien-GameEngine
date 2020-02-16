#pragma once

#include "Component.h"
#include "Globals.h"
#include "WwiseT.h"

class GameObject;

class AudioListener : public Component
{
public:
	AudioListener(GameObject* parent);
	~AudioListener();
	void Update(float dt);
	void UpdateListenerPos();

	void LoadComponent(JSONArraypack* to_load) override;


public:
	WwiseT::AudioSource* listener;
};
