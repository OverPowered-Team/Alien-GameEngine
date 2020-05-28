#include "TriggerAudioEvent.h"

TriggerAudioEvent::TriggerAudioEvent(): Alien()
{
}

TriggerAudioEvent::~TriggerAudioEvent()
{
}

void TriggerAudioEvent::Start()
{
	emitter = game_object->GetComponent<ComponentAudioEmitter>();
}

void TriggerAudioEvent::Update()
{
}

void TriggerAudioEvent::OnTriggerEnter(ComponentCollider* collider)
{
	if (!triggered && strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		emitter->StartSound();
		triggered = true;
	}
}

void TriggerAudioEvent::OnTriggerExit(ComponentCollider* collider)
{
}
