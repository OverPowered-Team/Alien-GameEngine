#include "GameManager.h"
#include "DialogueTriggerSimple.h"
#include "EventManager.h"
#include "DialogueManager.h"

DialogueTriggerSimple::DialogueTriggerSimple() : Alien()
{
}

DialogueTriggerSimple::~DialogueTriggerSimple()
{
}

void DialogueTriggerSimple::Start()
{
}

void DialogueTriggerSimple::Update()
{
	static bool lastHitToPlay = hitToPlay;

	if (lastHitToPlay != hitToPlay)
		EmitDialogue();
	lastHitToPlay = hitToPlay;

}

// TODO: we need to change this later
static std::string GetName(bool Geralt)
{
	return ((Geralt) ? "Geralt" : "Yennefer");
}

void DialogueTriggerSimple::OnTriggerEnter(ComponentCollider* collider)
{
	if (!specificEntity && (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) || (specificEntity && strcmp(collider->game_object_attached->GetName(), GetName(Geralt).c_str()) == 0))
		EmitDialogue();
}


void  DialogueTriggerSimple::EmitDialogue()
{
	/*LOG("Dialogue trigger activated, dialogue audio event name: %s", dialogue.audioData.eventName.c_str());
	eventManager->ReceiveDialogueEvent(this->dialogue);*/
	GameManager::instance->event_manager->ReceiveDialogueEvent(diaLogueIndex, volume);

	// TODO: we can't delete this, won't play sound? The object is unrelated with the emitter (XD)
	if (once)
		game_object->ToDelete();
}
