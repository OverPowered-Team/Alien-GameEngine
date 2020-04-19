#include "ReverbZone.h"
#include "../../ComponentAudioEmitter.h"

ReverbZone::ReverbZone() : Alien()
{
}

ReverbZone::~ReverbZone()
{
}

void ReverbZone::Start()
{
}

void ReverbZone::Update()
{
}

void ReverbZone::OnTrigger(ComponentCollider* collider)
{
}

void ReverbZone::OnTriggerEnter(ComponentCollider* collider)
{
	
	ComponentAudioEmitter* emitter = (ComponentAudioEmitter*)collider->game_object_attached->GetComponent(ComponentType::A_EMITTER);

	if (emitter != nullptr)
		emitter->SetReverb(strength, zone_name.c_str());
}

void ReverbZone::OnTriggerExit(ComponentCollider* collider)
{
	ComponentAudioEmitter* emitter = (ComponentAudioEmitter*)collider->game_object_attached->GetComponent(ComponentType::A_EMITTER);

	if (emitter != nullptr)
		emitter->SetReverb(0.f, zone_name.c_str());
}
