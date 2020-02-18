//#include "Application.h"
//#include "ModuleResources.h"
//#include "ModuleImporter.h"
//#include "ModuleInput.h"
//#include "ResourceAnimation.h"
//#include <fstream>
//#include <iomanip>
//
//#include "ResourceAnimatorController.h"
//
//
//
//ResourceAnimatorController::ResourceAnimatorController() : Resource()
//{
//	ed_context = ax::NodeEditor::CreateEditor();
//
//	for (int i = 0; i < NUM_TRIGGERS; ++i) {
//		triggers.push_back(false);
//	}
//
//	name = "Animator Controller";
//
//	default_state = nullptr;
//
//}
//
//ResourceAnimatorController::~ResourceAnimatorController()
//{
//	ax::NodeEditor::DestroyEditor(ed_context);
//}
//
//
//void ResourceAnimatorController::Update()
//{
//	if (Time::IsPlaying()) {
//		if (current_state)
//		{
//			UpdateState(current_state);
//		}
//		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
//			triggers[2] = true;
//		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
//			triggers[0] = true;
//		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_UP)
//			triggers[1] = true;
//	}
//}
//
//void ResourceAnimatorController::UpdateState(State* state)
//{
//	ResourceAnimation* animation = state->GetClip();
//
//	CheckTriggers();
//
//	if (animation && animation->GetDuration() > 0) {
//
//		state->time += Time::GetDT();
//
//		if (state->time >= animation->GetDuration()) {
//			if (!state->next_state) {
//				std::vector<Transition*> possible_transitions = FindTransitionsFromSourceState(state);
//				for (std::vector<Transition*>::iterator it = possible_transitions.begin(); it != possible_transitions.end(); ++it) {
//					if ((*it)->GetTrigger() == 0) {
//						state->next_state = (*it)->GetTarget();
//						state->fade_duration = (*it)->GetBlend();
//						break;
//					}
//				}
//			}
//			if (state->GetClip()->loops)
//				state->time = 0;
//			else
//				state->time = animation->GetDuration();
//		}
//
//	}
//
//	if (state->next_state) {
//
//		float to_end = state->fade_duration - state->fade_time;
//
//		if (to_end >= 0) {
//			
//			state->fade_time += Time::GetDT();
//			UpdateState(state->next_state);
//		}
//		else {
//			current_state = state->next_state;
//			state->next_state = nullptr;
//			state->time = 0;
//			state->fade_time = 0;
//			state->fade_duration = 0;
//		}
//	}
//}
//
//void ResourceAnimatorController::Stop()
//{
//	current_state = nullptr;
//}
//
//void ResourceAnimatorController::CheckTriggers()
//{
//	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
//		if ((*it)->GetTrigger() > 0)
//		{
//			if (triggers[(*it)->GetTrigger() - 1] == true) {
//				current_state->next_state = (*it)->GetTarget();
//				current_state->fade_duration = (*it)->GetBlend();
//				triggers[(*it)->GetTrigger() - 1] = false;
//			}
//		}
//	}
//}
//
//void ResourceAnimatorController::SaveAsset()
//{
//	nlohmann::json j_states;
//	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
//	{
//		nlohmann::json j_state = {
//			{"name", (*it)->GetName()},
//			{"speed", (*it)->GetSpeed()},
//			{"clip", (*it)->GetClip() ? (*it)->GetClip()->GetID() : 0}
//		};
//		j_states.push_back(j_state);
//	}
//
//	nlohmann::json j_transitions;
//	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it)
//	{
//		nlohmann::json j_transition = {
//			{ "source", (*it)->GetSource()->GetName() },
//			{ "target", (*it)->GetTarget()->GetName() },
//			{ "trigger", (*it)->GetTrigger() },
//			{ "blend", (*it)->GetBlend()}
//		};
//		j_transitions.push_back(j_transition);
//	}
//
//	nlohmann::json file = {
//		{ "name", name },
//		{ "states", j_states},
//		{ "transitions", j_transitions}
//	};
//
//	std::ofstream o(meta->original_file);
//	o << std::setw(4) << file << std::endl;
//
//	meta->id = id;
//	App->importer->anim_controller->Save(meta->original_file, this);
//}
//
//void ResourceAnimatorController::FreeMemory()
//{
//}
//
//bool ResourceAnimatorController::LoadMemory()
//{
//	char* buffer;
//	uint size = App->file_system->Load(meta_data_path.data(), &buffer);
//
//	if (size > 0)
//	{
//		char* cursor = buffer;
//
//		//Load name size
//		uint bytes = sizeof(uint);
//		uint name_size;
//		memcpy(&name_size, cursor, bytes);
//		cursor += bytes;
//
//		//Load name
//		bytes = name_size;
//		resource->name.resize(bytes);
//		memcpy(&resource->name[0], cursor, bytes);
//		cursor += bytes;
//
//		//Load transitions and states nums
//		bytes = sizeof(uint);
//		uint num_states;
//		memcpy(&num_states, cursor, bytes);
//		cursor += bytes;
//		uint num_transitions;
//		memcpy(&num_transitions, cursor, bytes);
//		cursor += bytes;
//
//		for (int i = 0; i < num_states; i++)
//		{
//			//Load name size
//			uint bytes = sizeof(uint);
//			name_size;
//			memcpy(&name_size, cursor, bytes);
//			cursor += bytes;
//			//Load name
//			bytes = name_size;
//			std::string tmp_name;
//			tmp_name.resize(name_size);
//			memcpy(&tmp_name[0], cursor, bytes);
//			cursor += bytes;
//
//			//Load clip id and speed
//			bytes = sizeof(uint);
//			uint clip_id;
//			memcpy(&clip_id, cursor, bytes);
//			cursor += bytes;
//			bytes = sizeof(float);
//			float speed;
//			memcpy(&speed, cursor, bytes);
//			cursor += bytes;
//
//			resource->AddState(tmp_name, (ResourceAnimation*)App->resources->GetAndReference(clip_id), speed);
//		}
//
//		for (int i = 0; i < num_transitions; ++i)
//		{
//			//Load name size
//			uint bytes = sizeof(uint);
//			name_size;
//			memcpy(&name_size, cursor, bytes);
//			cursor += bytes;
//			//Load name
//			bytes = name_size;
//			std::string tmp_source;
//			tmp_source.resize(name_size);
//			memcpy(&tmp_source[0], cursor, bytes);
//			cursor += bytes;
//
//			//Load name size
//			bytes = sizeof(uint);
//			name_size;
//			memcpy(&name_size, cursor, bytes);
//			cursor += bytes;
//			//Load name
//			bytes = name_size;
//			std::string tmp_target;
//			tmp_target.resize(name_size);
//			memcpy(&tmp_target[0], cursor, bytes);
//			cursor += bytes;
//
//			bytes = sizeof(uint);
//			uint tmp_trigger;
//			memcpy(&tmp_trigger, cursor, bytes);
//			cursor += bytes;
//
//			bytes = sizeof(float);
//			float tmp_blend;
//			memcpy(&tmp_blend, cursor, bytes);
//			cursor += bytes;
//
//			resource->AddTransition(resource->FindState(tmp_source), resource->FindState(tmp_target), tmp_blend, tmp_trigger);
//		}
//
//		LOG_ENGINE("Loaded Anim Controller %s", resource->GetExportedFile());
//
//		RELEASE_ARRAY(buffer);
//		return true;
//	}
//	return false
//}
//
//void ResourceAnimatorController::Play()
//{
//	if (default_state)
//	{
//		current_state = default_state;
//	}
//}
//
//void ResourceAnimatorController::Play(std::string state_name)
//{
//	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
//	{
//		if ((*it)->GetName() == state_name)
//			current_state = (*it);
//	}
//}
//
//bool ResourceAnimatorController::GetTransform(std::string channel_name, float3& position, Quat& rotation, float3& scale)
//{
//	if (current_state)
//	{
//		return GetTransformState(current_state, channel_name, position, rotation, scale);
//	}
//
//	return false;
//}
//
//bool ResourceAnimatorController::GetTransformState(State* state, std::string channel_name, float3& position, Quat& rotation, float3& scale)
//{
//	ResourceAnimation* animation = state->GetClip();
//
//	if (animation)
//	{
//		uint channel_index = animation->GetChannelIndex(channel_name);
//
//		if (channel_index < animation->num_channels) {
//
//			float3 next_position, next_scale;
//			Quat next_rotation;
//			float previous_key_time, next_key_time, t = 0;
//
//			float time_in_ticks = animation->start_tick + (state->time * animation->ticks_per_second);
//
//			if (animation->channels[channel_index].num_position_keys > 1)
//			{
//				for (int i = 0; i < animation->channels[channel_index].num_position_keys; i++)
//				{
//					if (time_in_ticks < animation->channels[channel_index].position_keys[i + 1].time) {
//						position = animation->channels[channel_index].position_keys[i].value;
//						next_position = animation->channels[channel_index].position_keys[i + 1].value;
//						next_key_time = animation->channels[channel_index].position_keys[i + 1].time;
//						t = (float)((double)time_in_ticks / next_key_time);
//						break;
//					}
//				}
//
//				position = float3::Lerp(position, next_position, t);
//
//			}
//			else
//				position = animation->channels[channel_index].position_keys[0].value;
//
//			if (animation->channels[channel_index].num_rotation_keys > 1)
//			{
//				for (int i = 0; i < animation->channels[channel_index].num_rotation_keys; i++)
//				{
//					if (time_in_ticks < animation->channels[channel_index].rotation_keys[i + 1].time) {
//						rotation = animation->channels[channel_index].rotation_keys[i].value;
//						next_rotation = animation->channels[channel_index].rotation_keys[i + 1].value;
//						next_key_time = animation->channels[channel_index].rotation_keys[i + 1].time;
//						t = (float)((double)time_in_ticks / next_key_time);
//						break;
//					}
//				}
//
//				rotation = Quat::Slerp(rotation, next_rotation, t);
//
//
//			}
//			else
//				rotation = animation->channels[channel_index].rotation_keys[0].value;
//
//
//
//			for (int i = 0; i < animation->channels[channel_index].num_scale_keys; i++)
//			{
//				if (time_in_ticks < animation->channels[channel_index].scale_keys[i + 1].time) {
//					scale = animation->channels[channel_index].scale_keys[i].value;
//					next_scale = animation->channels[channel_index].scale_keys[i + 1].value;
//					next_key_time = animation->channels[channel_index].scale_keys[i + 1].time;
//					t = (float)((double)time_in_ticks / next_key_time);
//					break;
//				}
//			}
//
//			scale = float3::Lerp(scale, next_scale, t);
//
//
//			if (state->next_state) {
//				float3 next_state_position, next_state_scale;
//				Quat next_state_rotation;
//
//				if (GetTransformState(state->next_state, channel_name, next_state_position, next_state_rotation, next_state_scale)) {
//					float fade_t = state->fade_time / state->fade_duration;
//
//					position = float3::Lerp(position, next_state_position, fade_t);
//					rotation = Quat::Slerp(rotation, next_state_rotation, fade_t);
//					scale = float3::Lerp(scale, next_state_scale, fade_t);
//				}
//			}
//
//			return true;
//		}
//		else
//			return false;
//	}
//	else
//		return false;
//}
//
//void ResourceAnimatorController::AddState(std::string name, ResourceAnimation* clip, float speed)
//{
//	State* new_state = new State(name, clip);
//	new_state->SetSpeed(speed);
//	states.push_back(new_state);
//
//	if (!default_state)
//		default_state = new_state;
//}
//
//void ResourceAnimatorController::RemoveState(std::string name)
//{
//	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
//		if ((*it)->GetSource()->GetName() == name || (*it)->GetTarget()->GetName() == name) {
//			delete (*it);
//			it = transitions.erase(it);
//			break;
//		}
//	}
//
//	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
//		if ((*it)->GetName() == name) {
//			delete (*it);
//			it = states.erase(it);
//			break;
//		}
//	}
//}
//
//State* ResourceAnimatorController::FindState(std::string name)
//{
//	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
//		if ((*it)->GetName() == name)
//			return (*it);
//	}
//}
//
//State* ResourceAnimatorController::FindState(uint id)
//{
//	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
//		if ((*it)->id == id)
//			return (*it);
//	}
//}
//
//State* ResourceAnimatorController::FindStateFromPinId(uint pin_id)
//{
//	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
//		if ((*it)->pin_in_id == pin_id || (*it)->pin_out_id == pin_id)
//			return (*it);
//	}
//}
//
//void ResourceAnimatorController::AddTransition(State* source, State* target, float blend)
//{
//	Transition* new_transition = new Transition(source, target, blend);
//
//	transitions.push_back(new_transition);
//}
//
//void ResourceAnimatorController::AddTransition(State* source, State* target, float blend, uint trigger)
//{
//	Transition* new_transition = new Transition(source, target, blend);
//	new_transition->SetTrigger(trigger);
//
//	transitions.push_back(new_transition);
//}
//
//void ResourceAnimatorController::RemoveTransition(std::string source_name, std::string target_name)
//{
//	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
//		if ((*it)->GetSource()->GetName() == source_name && (*it)->GetTarget()->GetName() == target_name) {
//			delete (*it);
//			it = transitions.erase(it);
//			break;
//		}
//	}
//}
//
//std::vector<Transition*> ResourceAnimatorController::FindTransitionsFromSourceState(State* state)
//{
//	std::vector<Transition*> ret;
//	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
//		if ((*it)->GetSource() == state) {
//			ret.push_back((*it));
//		}
//	}
//
//	return ret;
//}
//
//ax::NodeEditor::EditorContext* ResourceAnimatorController::GetEditorContext()
//{
//	return ed_context;
//}
//
//std::string ResourceAnimatorController::GetTypeString() const
//{
//	return "Animation Controller";
//}
//
//std::string ResourceAnimatorController::GetName()
//{
//	return name;
//}
//
//void ResourceAnimatorController::UnLoad()
//{
//	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
//	{
//		if ((*it)->GetClip())
//			(*it)->GetClip()->RemoveReference();
//		delete (*it);
//	}
//	states.clear();
//	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it)
//	{
//		delete (*it);
//	}
//	transitions.clear();
//}
//
//void ResourceAnimatorController::Load()
//{
//	App->importer->anim_controller->Load(this);
//}
//
//void ResourceAnimatorController::Reset()
//{
//}
//
//State::State()
//{
//}
//
//State::State(std::string name, ResourceAnimation* clip)
//{
//	this->name = name;
//	this->clip = clip;
//}
//
//void State::SetSpeed(float speed)
//{
//	this->speed = speed;
//}
//
//void State::SetName(std::string name)
//{
//	this->name = name;
//}
//
//void State::SetClip(ResourceAnimation* clip)
//{
//	clip->AddReference();
//	this->clip = clip;
//}
//
//float State::GetSpeed()
//{
//	return speed;
//}
//
//std::string State::GetName()
//{
//	return name;
//}
//
//ResourceAnimation* State::GetClip()
//{
//	return clip;
//}
//
//Transition::Transition()
//{
//}
//
//Transition::Transition(State* source, State* target, float blend)
//{
//	this->source = source;
//	this->target = target;
//	this->blend = blend;
//}
//
//void Transition::SetSource(State* source)
//{
//	this->source = source;
//}
//
//void Transition::SetTarget(State* target)
//{
//	this->target = target;
//}
//
//void Transition::SetTrigger(uint trigger)
//{
//	this->trigger = trigger;
//}
//
//void Transition::SetBlend(float blend)
//{
//	this->blend = blend;
//}
//
//State* Transition::GetSource()
//{
//	return source;
//}
//
//State* Transition::GetTarget()
//{
//	return target;
//}
//
//uint Transition::GetTrigger()
//{
//	return trigger;
//}
//
//float Transition::GetBlend()
//{
//	return blend;
//}
//
