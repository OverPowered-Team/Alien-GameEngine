#include "ComponentAudioEmitter.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/MathGeoLib.h"

ComponentAudioEmitter::ComponentAudioEmitter(GameObject * parent) : Component(parent)
{
	type = ComponentType::A_EMITTER;
	source = App->audio->CreateSoundEmitter("Emitter");
	App->audio->emitters.push_back(this);
}

void ComponentAudioEmitter::Update(float dt)
{
	UpdateSourcePos();
	
	if (timer.Read() / 1000 >= time_to_swap) {
		if (song == 1) {
			source->ChangeState("swap_music", "state2");
			song = 2;
			timer.Start();
		}
		else {
			source->ChangeState("swap_music", "state1");
			song = 1;
			timer.Start();
		}
	}
}

ComponentAudioEmitter::~ComponentAudioEmitter()
{
	RELEASE(source);
}

void ComponentAudioEmitter::ChangeVolume(float new_volume)
{
	if (mute != true) {
		volume = new_volume;
		source->SetVolume(volume);
	}
}

void ComponentAudioEmitter::Mute(bool mute)
{
	mute = mute;
	if (mute == true)
		source->SetVolume(0);
	else if (mute == false)
		source->SetVolume(volume);
}

void ComponentAudioEmitter::ChangeTimeToSwap(float new_time)
{
	time_to_swap = new_time;
}

void ComponentAudioEmitter::StartSound()
{	
	source->PlayEventByName(audio_name.c_str());
	timer.Start();	
}

void ComponentAudioEmitter::UpdateSourcePos()
{
	/*ComponentTransform* transformation = game_object_attached->GetComponentTransform();

	if (transformation != nullptr)
	{
		math::Quat rot = transformation->rotation;

		math::float3 vector_pos = transformation->position;

		source->SetSourcePos(vector_pos.x, vector_pos.y, vector_pos.z, 0, 0, 1, 0, 1, 0);
	}*/
}
/*
void ComponentAudioEmitter::SaveEmitter(JSON_Array* componentsObj) const
{
	// TODO: Adapt to AlienEngine SaveComponent
	JSON_Value* component = json_value_init_object();
	JSON_Object* componentObj = json_value_get_object(component);
	json_object_set_number(componentObj, "Type:", this->type);
	json_object_set_number(componentObj, "Volume:", this->volume);
	json_object_set_number(componentObj, "TimeToSwap:", this->time_to_swap);
	json_object_set_number(componentObj, "Song:", this->song);
	json_object_set_boolean(componentObj, "Mute:", this->mute);
	json_object_set_string(componentObj, "AudioName:", this->audio_name.c_str());

	json_array_append_value(componentsObj, component);
}

void ComponentAudioEmitter::LoadEmitter(JSON_Object* obj, GameObject* go)
{
	App->scene_intro->gameobject_scene->audio_emitter == nullptr;
	App->scene_intro->GOPath->audio_emitter == nullptr;
	go->audio_emitter->volume = json_object_get_number(obj, "Volume:");
	go->audio_emitter->time_to_swap = json_object_get_number(obj, "TimeToSwap:");
	go->audio_emitter->song = json_object_get_number(obj, "Song:");
	go->audio_emitter->mute = json_object_get_boolean(obj, "Mute:");
	go->audio_emitter->audio_name = json_object_get_string(obj, "AudioName:");
}
*/