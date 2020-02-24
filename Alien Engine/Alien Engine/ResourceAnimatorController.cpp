#include "Application.h"
#include "ModuleResources.h"
#include "ModuleImporter.h"
#include "ModuleInput.h"
#include "ResourceAnimation.h"
#include <fstream>
#include <iomanip>

#include "ResourceAnimatorController.h"




ResourceAnimatorController::ResourceAnimatorController() : Resource()
{
	type = ResourceType::RESOURCE_ANIMATOR_CONTROLLER;
	ed_context = ax::NodeEditor::CreateEditor();

	for (int i = 0; i < NUM_TRIGGERS; ++i) {
		triggers.push_back(false);
	}

	name = "Animator Controller";

	default_state = nullptr;

}

ResourceAnimatorController::~ResourceAnimatorController()
{
	ax::NodeEditor::DestroyEditor(ed_context);
}

std::vector < std::pair <std::string, bool>>  ResourceAnimatorController::GetBoolParameters()
{
	return bool_parameters;
}

std::vector < std::pair <std::string, float>>  ResourceAnimatorController::GetFloatParameters()
{
	return float_parameters;
}

std::vector < std::pair <std::string, int>>  ResourceAnimatorController::GetIntParameters()
{
	return int_parameters;
}

void ResourceAnimatorController::AddBoolParameter()
{
	bool_parameters.push_back({"NewBool", false });
}

void ResourceAnimatorController::AddFloatParameter()
{
	float_parameters.push_back({"NewFloat", 0.0f });
}

void ResourceAnimatorController::AddIntParameter()
{
	int_parameters.push_back({"NewInt", 0 });
}

void ResourceAnimatorController::SetBool(std::string name, bool value)
{
	for (int i = 0; i < bool_parameters.size(); i++) {
		if (bool_parameters[i].first == name)
			bool_parameters[i].second = value;
	}
}

void ResourceAnimatorController::SetFloat(std::string name, float value)
{
	for (int i = 0; i < float_parameters.size(); i++) {
		if (float_parameters[i].first == name)
			float_parameters[i].second = value;
	}
}

void ResourceAnimatorController::SetInt(std::string name, int value)
{
	for (int i = 0; i < int_parameters.size(); i++) {
		if (int_parameters[i].first == name)
			int_parameters[i].second = value;
	}
}


void ResourceAnimatorController::Update()
{
	if (Time::IsPlaying()) {
		if (current_state)
		{
			UpdateState(current_state);
		}
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			triggers[2] = true;
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
			triggers[0] = true;
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_UP)
			triggers[1] = true;
	}
}

void ResourceAnimatorController::UpdateState(State* state)
{
	ResourceAnimation* animation = state->GetClip();

	CheckTriggers();

	if (animation && animation->GetDuration() > 0) {

		state->time += Time::GetDT();

		if (state->time >= animation->GetDuration()) {
			if (!state->next_state) {
				std::vector<Transition*> possible_transitions = FindTransitionsFromSourceState(state);
				for (std::vector<Transition*>::iterator it = possible_transitions.begin(); it != possible_transitions.end(); ++it) {
					if ((*it)->GetTrigger() == 0) {
						state->next_state = (*it)->GetTarget();
						state->fade_duration = (*it)->GetBlend();
						break;
					}
				}
			}
			if (state->GetClip()->loops)
				state->time = 0;
			else
				state->time = animation->GetDuration();
		}

	}

	if (state->next_state) {

		float to_end = state->fade_duration - state->fade_time;

		if (to_end >= 0) {
			
			state->fade_time += Time::GetDT();
			UpdateState(state->next_state);
		}
		else {
			current_state = state->next_state;
			state->next_state = nullptr;
			state->time = 0;
			state->fade_time = 0;
			state->fade_duration = 0;
		}
	}
}

void ResourceAnimatorController::Stop()
{
	current_state = nullptr;
}

void ResourceAnimatorController::CheckTriggers()
{
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
		if ((*it)->GetTrigger() > 0)
		{
			if (triggers[(*it)->GetTrigger() - 1] == true) {
				current_state->next_state = (*it)->GetTarget();
				current_state->fade_duration = (*it)->GetBlend();
				triggers[(*it)->GetTrigger() - 1] = false;
			}
		}
	}
}

bool ResourceAnimatorController::SaveAsset(const u64& force_id)
{
	if (force_id == 0)
		ID = App->resources->GetRandomID();
	else
		ID = force_id;
	
	path = std::string(ANIM_CONTROLLER_FOLDER + name + ".animController");

	JSON_Value* asset_value = json_value_init_object();
	JSON_Object* asset_object = json_value_get_object(asset_value);
	json_serialize_to_file_pretty(asset_value, path.data());

	JSONfilepack* asset = new JSONfilepack(path, asset_object, asset_value);
	asset->StartSave();
	asset->SetString("Controller.Name", name);
	asset->SetNumber("Controller.NumStates", states.size());
	asset->SetNumber("Controller.NumTransitions", transitions.size());
	
	JSONArraypack* states_array = asset->InitNewArray("Controller.States");
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		states_array->SetAnotherNode();
		states_array->SetString("Name", (*it)->GetName());
		states_array->SetNumber("Speed", (*it)->GetSpeed());
		states_array->SetNumber("Clip", (*it)->GetClip() ? (*it)->GetClip()->GetID() : 0);
	}

	JSONArraypack* transitions_array = asset->InitNewArray("Controller.Transitions");
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it)
	{
		transitions_array->SetAnotherNode();
		transitions_array->SetString("Source", (*it)->GetSource()->GetName());
		transitions_array->SetString("Target", (*it)->GetTarget()->GetName());
		transitions_array->SetNumber("Trigger", (*it)->GetTrigger());
		transitions_array->SetNumber("Blend", (*it)->GetBlend());
	}

	asset->FinishSave();
	CreateMetaData(ID);

	return true;
}

void ResourceAnimatorController::FreeMemory()
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		if ((*it)->GetClip())
			(*it)->GetClip()->DecreaseReferences();
		delete (*it);
	}
	states.clear();
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it)
	{
		delete (*it);
	}
	transitions.clear();
}
bool ResourceAnimatorController::LoadMemory()
{
	char* buffer;
	uint size = App->file_system->Load(meta_data_path.data(), &buffer);

	if (size > 0)
	{
		char* cursor = buffer;

		//Load name size
		uint bytes = sizeof(uint);
		uint name_size;
		memcpy(&name_size, cursor, bytes);
		cursor += bytes;

		//Load name
		bytes = name_size;
		name.resize(bytes);
		memcpy(&name[0], cursor, bytes);
		cursor += bytes;

		//Load transitions and states nums
		bytes = sizeof(uint);
		uint num_states;
		memcpy(&num_states, cursor, bytes);
		cursor += bytes;
		uint num_transitions;
		memcpy(&num_transitions, cursor, bytes);
		cursor += bytes;

		for (int i = 0; i < num_states; i++)
		{
			//Load name size
			uint bytes = sizeof(uint);
			name_size;
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;
			//Load name
			bytes = name_size;
			std::string tmp_name;
			tmp_name.resize(name_size);
			memcpy(&tmp_name[0], cursor, bytes);
			cursor += bytes;

			//Load clip id and speed
			bytes = sizeof(uint);
			uint clip_id;
			memcpy(&clip_id, cursor, bytes);
			cursor += bytes;
			bytes = sizeof(float);
			float speed;
			memcpy(&speed, cursor, bytes);
			cursor += bytes;

			AddState(tmp_name, (ResourceAnimation*)App->resources->GetResourceWithID(clip_id), speed);
		}

		for (int i = 0; i < num_transitions; ++i)
		{
			//Load name size
			uint bytes = sizeof(uint);
			name_size;
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;
			//Load name
			bytes = name_size;
			std::string tmp_source;
			tmp_source.resize(name_size);
			memcpy(&tmp_source[0], cursor, bytes);
			cursor += bytes;

			//Load name size
			bytes = sizeof(uint);
			name_size;
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;
			//Load name
			bytes = name_size;
			std::string tmp_target;
			tmp_target.resize(name_size);
			memcpy(&tmp_target[0], cursor, bytes);
			cursor += bytes;

			bytes = sizeof(uint);
			uint tmp_trigger;
			memcpy(&tmp_trigger, cursor, bytes);
			cursor += bytes;

			bytes = sizeof(float);
			float tmp_blend;
			memcpy(&tmp_blend, cursor, bytes);
			cursor += bytes;

			AddTransition(FindState(tmp_source), FindState(tmp_target), tmp_blend, tmp_trigger);
		}
		LOG_ENGINE("Loaded Anim Controller %s", meta_data_path.data());

		RELEASE_ARRAY(buffer);
		return true;
	}
	return false;
}

bool ResourceAnimatorController::ReadBaseInfo(const char* assets_file_path)
{
	bool ret = true;

	path = std::string(assets_file_path);
	std::string alien_path = App->file_system->GetPathWithoutExtension(assets_file_path) + "_meta.alien";

	JSON_Value* value = json_parse_file(alien_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(alien_path, object, value);

		ID = std::stoull(meta->GetString("Meta.ID"));

		delete meta;

		meta_data_path = LIBRARY_ANIM_CONTROLLERS_FOLDER + std::to_string(ID) + ".alienAnimController";
		char* buffer;
		uint size = App->file_system->Load(meta_data_path.data(), &buffer);

		if (size > 0)
		{
			char* cursor = buffer;

			//Load name size
			uint bytes = sizeof(uint);
			uint name_size;
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;

			//Load name
			bytes = name_size;
			name.resize(bytes);
			memcpy(&name[0], cursor, bytes);
			cursor += bytes;

			//Load transitions and states nums
			bytes = sizeof(uint);
			uint num_states;
			memcpy(&num_states, cursor, bytes);
			cursor += bytes;
			uint num_transitions;
			memcpy(&num_transitions, cursor, bytes);
			cursor += bytes;
		}
		else
			return false;
	}
	else
		return false;

	App->resources->AddResource(this);

	return ret;
}

void ResourceAnimatorController::ReadLibrary(const char* meta_data)
{
	this->meta_data_path = meta_data;
	ID = std::stoull(App->file_system->GetBaseFileName(meta_data_path.data()));
	App->resources->AddResource(this);
}

bool ResourceAnimatorController::CreateMetaData(const u64& force_id)
{
	if (force_id == 0)
		ID = App->resources->GetRandomID();
	else
		ID = force_id;

	meta_data_path = std::string(LIBRARY_ANIM_CONTROLLERS_FOLDER + std::to_string(ID) + ".alienAnimController");

	//SAVE META FILE
	std::string meta_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");

	JSON_Value* meta_value = json_value_init_object();
	JSON_Object* meta_object = json_value_get_object(meta_value);
	json_serialize_to_file_pretty(meta_value, meta_path.data());

	if (meta_value != nullptr && meta_object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_path, meta_object, meta_value);
		meta->StartSave();
		meta->SetString("Meta.ID", std::to_string(ID));
		meta->FinishSave();
	}

	//SAVE LIBRARY FILE
	uint size = sizeof(uint) + name.size() + sizeof(uint) * 2;
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		size += sizeof(uint) + (*it)->GetName().size() + sizeof(u64) + sizeof(float);
	}
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it)
	{
		size += sizeof(uint) + (*it)->GetSource()->GetName().size() + sizeof(uint) + (*it)->GetTarget()->GetName().size() + sizeof(uint) + sizeof(float);
	}
	// Allocate
	char* data = new char[size];
	char* cursor = data;

	// Store name size and name
	uint bytes = sizeof(uint);
	uint name_size = name.size();
	memcpy(cursor, &name_size, bytes);
	cursor += bytes;
	bytes = name.size();
	memcpy(cursor, name.c_str(), bytes);
	cursor += bytes;

	//Store transitions and states nums
	bytes = sizeof(uint);
	uint num_states = states.size();
	memcpy(cursor, &num_states, bytes);
	cursor += bytes;
	uint num_transitions = transitions.size();
	memcpy(cursor, &num_transitions, bytes);
	cursor += bytes;

	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		// Store name size and name
		bytes = sizeof(uint);
		name_size = (*it)->GetName().size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = name_size;
		memcpy(cursor, (*it)->GetName().data(), bytes);
		cursor += bytes;

		//Store clip id and speed
		bytes = sizeof(u64);
		memcpy(cursor, &(*it)->GetClip()->GetID(), bytes);
		cursor += bytes;
		bytes = sizeof(float);
		float st_speed = (*it)->GetSpeed();
		memcpy(cursor, &st_speed, bytes);
		cursor += bytes;
	}

	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it)
	{
		// Store name size and name
		bytes = sizeof(uint);
		name_size = (*it)->GetSource()->GetName().size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = name_size;
		memcpy(cursor, (*it)->GetSource()->GetName().data(), bytes);
		cursor += bytes;

		// Store name size and name
		bytes = sizeof(uint);
		name_size = (*it)->GetTarget()->GetName().size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = name_size;
		memcpy(cursor, (*it)->GetTarget()->GetName().data(), bytes);
		cursor += bytes;

		bytes = sizeof(uint);
		uint trigger_id = (*it)->GetTrigger();
		memcpy(cursor, &trigger_id, bytes);
		cursor += bytes;

		bytes = sizeof(float);
		float tr_blend = (*it)->GetBlend();
		memcpy(cursor, &tr_blend, bytes);
		cursor += bytes;
	}

	App->file_system->Save(meta_data_path.data(), data, size);
	App->resources->AddResource(this);
	RELEASE_ARRAY(data);

	return true;
}

bool ResourceAnimatorController::DeleteMetaData()
{
	remove(meta_data_path.data());

	std::vector<Resource*>::iterator position = std::find(App->resources->resources.begin(), App->resources->resources.end(), static_cast<Resource*>(this));
	if (position != App->resources->resources.end())
		App->resources->resources.erase(position);

	delete this;

	return true;
}

void ResourceAnimatorController::Play()
{
	if (default_state)
	{
		current_state = default_state;
	}
}

void ResourceAnimatorController::Play(std::string state_name)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		if ((*it)->GetName() == state_name)
			current_state = (*it);
	}
}

bool ResourceAnimatorController::GetTransform(std::string channel_name, float3& position, Quat& rotation, float3& scale)
{
	if (current_state)
	{
		return GetTransformState(current_state, channel_name, position, rotation, scale);
	}

	return false;
}

bool ResourceAnimatorController::GetTransformState(State* state, std::string channel_name, float3& position, Quat& rotation, float3& scale)
{
	ResourceAnimation* animation = state->GetClip();

	if (animation)
	{
		uint channel_index = animation->GetChannelIndex(channel_name);

		if (channel_index < animation->num_channels) {

			float3 next_position, next_scale;
			Quat next_rotation;
			float previous_key_time, next_key_time, t = 0;

			float time_in_ticks = animation->start_tick + (state->time * animation->ticks_per_second);

			if (animation->channels[channel_index].num_position_keys > 1)
			{
				for (int i = 0; i < animation->channels[channel_index].num_position_keys; i++)
				{
					if (time_in_ticks < animation->channels[channel_index].position_keys[i + 1].time) {
						position = animation->channels[channel_index].position_keys[i].value;
						next_position = animation->channels[channel_index].position_keys[i + 1].value;
						next_key_time = animation->channels[channel_index].position_keys[i + 1].time;
						t = (float)((double)time_in_ticks / next_key_time);
						break;
					}
				}

				position = float3::Lerp(position, next_position, t);

			}
			else
				position = animation->channels[channel_index].position_keys[0].value;

			if (animation->channels[channel_index].num_rotation_keys > 1)
			{
				for (int i = 0; i < animation->channels[channel_index].num_rotation_keys; i++)
				{
					if (time_in_ticks < animation->channels[channel_index].rotation_keys[i + 1].time) {
						rotation = animation->channels[channel_index].rotation_keys[i].value;
						next_rotation = animation->channels[channel_index].rotation_keys[i + 1].value;
						next_key_time = animation->channels[channel_index].rotation_keys[i + 1].time;
						t = (float)((double)time_in_ticks / next_key_time);
						break;
					}
				}

				rotation = Quat::Slerp(rotation, next_rotation, t);


			}
			else
				rotation = animation->channels[channel_index].rotation_keys[0].value;



			for (int i = 0; i < animation->channels[channel_index].num_scale_keys; i++)
			{
				if (time_in_ticks < animation->channels[channel_index].scale_keys[i + 1].time) {
					scale = animation->channels[channel_index].scale_keys[i].value;
					next_scale = animation->channels[channel_index].scale_keys[i + 1].value;
					next_key_time = animation->channels[channel_index].scale_keys[i + 1].time;
					t = (float)((double)time_in_ticks / next_key_time);
					break;
				}
			}

			scale = float3::Lerp(scale, next_scale, t);


			if (state->next_state) {
				float3 next_state_position, next_state_scale;
				Quat next_state_rotation;

				if (GetTransformState(state->next_state, channel_name, next_state_position, next_state_rotation, next_state_scale)) {
					float fade_t = state->fade_time / state->fade_duration;

					position = float3::Lerp(position, next_state_position, fade_t);
					rotation = Quat::Slerp(rotation, next_state_rotation, fade_t);
					scale = float3::Lerp(scale, next_state_scale, fade_t);
				}
			}

			return true;
		}
		else
			return false;
	}
	else
		return false;
}

void ResourceAnimatorController::AddState(std::string name, ResourceAnimation* clip, float speed)
{
	State* new_state = new State(name, clip);
	new_state->SetSpeed(speed);
	states.push_back(new_state);

	if (!default_state)
		default_state = new_state;
}

void ResourceAnimatorController::RemoveState(std::string name)
{
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
		if ((*it)->GetSource()->GetName() == name || (*it)->GetTarget()->GetName() == name) {
			delete (*it);
			it = transitions.erase(it);
			break;
		}
	}

	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->GetName() == name) {
			delete (*it);
			it = states.erase(it);
			break;
		}
	}
}

State* ResourceAnimatorController::FindState(std::string name)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->GetName() == name)
			return (*it);
	}
}

State* ResourceAnimatorController::FindState(uint id)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->id == id)
			return (*it);
	}
}

State* ResourceAnimatorController::FindStateFromPinId(uint pin_id)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->pin_in_id == pin_id || (*it)->pin_out_id == pin_id)
			return (*it);
	}
}

void ResourceAnimatorController::AddTransition(State* source, State* target, float blend)
{
	Transition* new_transition = new Transition(source, target, blend);

	transitions.push_back(new_transition);
}

void ResourceAnimatorController::AddTransition(State* source, State* target, float blend, uint trigger)
{
	Transition* new_transition = new Transition(source, target, blend);
	new_transition->SetTrigger(trigger);

	transitions.push_back(new_transition);
}

void ResourceAnimatorController::RemoveTransition(std::string source_name, std::string target_name)
{
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
		if ((*it)->GetSource()->GetName() == source_name && (*it)->GetTarget()->GetName() == target_name) {
			delete (*it);
			it = transitions.erase(it);
			break;
		}
	}
}

std::vector<Transition*> ResourceAnimatorController::FindTransitionsFromSourceState(State* state)
{
	std::vector<Transition*> ret;
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
		if ((*it)->GetSource() == state) {
			ret.push_back((*it));
		}
	}

	return ret;
}

ax::NodeEditor::EditorContext* ResourceAnimatorController::GetEditorContext()
{
	return ed_context;
}

std::string ResourceAnimatorController::GetTypeString() const
{
	return "Animation Controller";
}

std::string ResourceAnimatorController::GetName()
{
	return name;
}

void ResourceAnimatorController::Reset()
{
}

State::State()
{
}

State::State(std::string name, ResourceAnimation* clip)
{
	this->name = name;
	this->clip = clip;
}

void State::SetSpeed(float speed)
{
	this->speed = speed;
}

void State::SetName(std::string name)
{
	this->name = name;
}

void State::SetClip(ResourceAnimation* clip)
{
	clip->IncreaseReferences();
	this->clip = clip;
}

float State::GetSpeed()
{
	return speed;
}

std::string State::GetName()
{
	return name;
}

ResourceAnimation* State::GetClip()
{
	return clip;
}

Transition::Transition()
{
}

Transition::Transition(State* source, State* target, float blend)
{
	this->source = source;
	this->target = target;
	this->blend = blend;
}

void Transition::SetSource(State* source)
{
	this->source = source;
}

void Transition::SetTarget(State* target)
{
	this->target = target;
}

void Transition::SetTrigger(uint trigger)
{
	this->trigger = trigger;
}

void Transition::SetBlend(float blend)
{
	this->blend = blend;
}

State* Transition::GetSource()
{
	return source;
}

State* Transition::GetTarget()
{
	return target;
}

uint Transition::GetTrigger()
{
	return trigger;
}

float Transition::GetBlend()
{
	return blend;
}

