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
	App->audio->emitters.push_back(this);
}

void ComponentAudioEmitter::Update()
{
	if (Time::state == Time::GameState::NONE) {
		if (play_mode) {
			source->StopEventByName(audio_name.c_str());
			play_mode = false;
		}
	}
	else if (Time::state == Time::GameState::PLAY) {
		if (!play_mode && play_on_awake) {
			StartSound();
			play_mode = true;
		}
	}
}

ComponentAudioEmitter::~ComponentAudioEmitter()
{
	source->StopEventByName(audio_name.c_str());
	App->audio->emitters.remove(this);
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
	if (mute)
		source->SetVolume(0.F);
	else
		source->SetVolume(volume);
	this->mute = mute;
}

//void ComponentAudioEmitter::ChangeTimeToSwap(float new_time)
//{
//	time_to_swap = new_time;
//}

void ComponentAudioEmitter::StartSound()
{	
	source->PlayEventByName(audio_name.c_str());
	//timer.Start();	
}

void ComponentAudioEmitter::UpdateSourcePos()
{
	ComponentTransform* transformation = game_object_attached->GetComponentTransform();

	if (transformation != nullptr)
	{
		//math::Quat rot = transformation->GetGlobalRotation();

		math::float3 vector_pos = transformation->GetGlobalPosition();

		source->SetSourcePos(vector_pos.x, vector_pos.y, vector_pos.z, 0, 0, 1, 0, 1, 0);
	}
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
	ImGui::PushID(this);

	if (ImGui::Checkbox("##CmpActive", &enabled)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		if (!enabled)
			OnDisable();
		else
			OnEnable();
	}
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Audio Emitter", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Audio Clip");
		ImGui::SameLine();
		static char buf[30];
		if (ImGui::InputText("Event name", buf, 30, ImGuiInputTextFlags_EnterReturnsTrue)) // TODO: Change inputText by a combo with all events 
			audio_name.assign(buf);
		/*if (ImGui::BeginCombo("Clip", "NONE")) {

		}*/
		ImGui::NewLine();
		if (ImGui::Checkbox("Mute", &mute))
			Mute(mute);
		ImGui::Checkbox("PlayOnAwake", &play_on_awake);
		ImGui::Checkbox("Loop", &loop);
		if (ImGui::SliderFloat("Volume", &volume, 0.F, 1.F))
			ChangeVolume(volume);
	}

	ImGui::Separator();
	ImGui::PopID();
	return true;
}