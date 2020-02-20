#include "ComponentAudioEmitter.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ComponentTransform.h"
#include "ReturnZ.h"
#include "MathGeoLib/include/MathGeoLib.h"

ComponentAudioEmitter::ComponentAudioEmitter(GameObject * parent) : Component(parent)
{
	type = ComponentType::A_EMITTER;
	source = App->audio->CreateSoundEmitter("Emitter");
	App->audio->audios.push_back(this);

}

void ComponentAudioEmitter::Update(float dt)
{
	//UpdateSourcePos();
	//
	//if (timer.Read() / 1000 >= time_to_swap) {
	//	if (song == 1) {
	//		source->ChangeState("swap_music", "state2");
	//		song = 2;
	//		timer.Start();
	//	}
	//	else {
	//		source->ChangeState("swap_music", "state1");
	//		song = 1;
	//		timer.Start();
	//	}
	//}
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

//void ComponentAudioEmitter::ChangeTimeToSwap(float new_time)
//{
//	time_to_swap = new_time;
//}

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
void ComponentAudioEmitter::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Volume", volume);
	to_save->SetBoolean("Mute", mute);
}
void ComponentAudioEmitter::LoadComponent(JSONArraypack* to_load)
{
	ID = std::stoull(to_load->GetString("ID"));
	enabled = to_load->GetBoolean("Enabled");
	volume = to_load->GetNumber("Volume");
	mute = to_load->GetNumber("Mute");
}
bool ComponentAudioEmitter::DrawInspector()
{
	static bool en;
	ImGui::PushID(this);
	en = enabled;
	if (ImGui::Checkbox("##CmpActive", &en)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = en;
		if (!enabled)
			OnDisable();
		else
			OnEnable();
	}
	ImGui::PopID();
	return true;
}