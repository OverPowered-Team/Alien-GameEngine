#include "ComponentAudioEmitter.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/MathGeoLib.h"

AudioEmitter::AudioEmitter(GameObject * parent) : Component(parent)
{
	type = ComponentType::A_EMITTER;

	source = App->audio->CreateSoundEmitter("Emitter");
	App->audio->audios.push_back(this);

}

void AudioEmitter::Update(float dt)
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

AudioEmitter::~AudioEmitter()
{
	RELEASE(source);
}

void AudioEmitter::ChangeVolume(float new_volume)
{
	if (mute != true) {
		volume = new_volume;
		source->SetVolume(volume);
	}
}

void AudioEmitter::Mute(bool mute)
{
	this->mute = mute;
	if (mute == true)
		source->SetVolume(0);
	else if (mute == false)
		source->SetVolume(volume);

}

void AudioEmitter::ChangeTimeToSwap(float new_time)
{
	time_to_swap = new_time;
}

void AudioEmitter::StartSound()
{	
	source->PlayEventByName(audio_name.c_str());
	timer.Start();	
}

void AudioEmitter::UpdateSourcePos()
{
	ComponentTransform *transformation = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);

	if (transformation != nullptr)
	{
		math::Quat rot = transformation->GetGlobalRotation();

		math::float3 vector_pos = transformation->GetGlobalPosition();

		source->SetSourcePos(vector_pos.x, vector_pos.y, vector_pos.z, 0, 0, 1, 0, 1, 0);
	}
}

void AudioEmitter::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("Volume", volume);
	to_save->SetNumber("TimeToSwap", time_to_swap);
	to_save->SetNumber("Song", song);
	to_save->SetBoolean("Mute", mute);
	to_save->SetString("AudioName", audio_name.c_str());
}

void AudioEmitter::LoadComponent(JSONArraypack* to_load)
{
	volume = to_load->GetNumber("Volume");
	time_to_swap = to_load->GetNumber("TimeToSwap");
	song = to_load->GetNumber("Song");
	mute = to_load->GetBoolean("Mute");
	audio_name.assign(to_load->GetString("AudioName"));
}
