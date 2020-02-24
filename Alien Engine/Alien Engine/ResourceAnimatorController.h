#pragma once
#ifndef _ANIMATOR_CONTROLLER_H_
#define _ANIMATOR_CONTROLLER_H_

#include "Resource.h"
#include "Globals.h"
#include "NodeEditor/Include/imgui_node_editor.h"

#include <vector>

#define NUM_TRIGGERS 5

class ResourceAnimation;

class State
{
private:
	std::string name;
	float speed = 1.0;
	ResourceAnimation* clip = nullptr;

public:
	uint pin_in_id, pin_out_id, id;
	float time = 0;
	float fade_duration = 0, fade_time = 0;
	State* next_state = nullptr;

public:
	State();
	State(std::string name, ResourceAnimation* clip);

public:
	void SetName(std::string name);
	void SetClip(ResourceAnimation* clip);
	void SetSpeed(float speed);

	float GetSpeed();
	std::string GetName();
	ResourceAnimation* GetClip();
};

class Transition
{
private:
	State* source;
	State* target;
	uint trigger = 0;
	float blend = 2;

public:
	Transition();
	Transition(State* source, State* target, float blend);

public:
	void SetSource(State* source);
	void SetTarget(State* target);
	void SetTrigger(uint trigger);
	void SetBlend(float blend);

	State* GetSource();
	State* GetTarget();
	uint GetTrigger();
	float GetBlend();
};


class ResourceAnimatorController : public Resource
{
private:
	State* current_state = nullptr;
	std::vector<State*> states;
	std::vector<Transition*> transitions;
	State* default_state = nullptr;
	std::vector<bool> triggers;

	std::vector <std::pair <std::string, int>> int_parameters;
	std::vector < std::pair <std::string, float>> float_parameters;
	std::vector < std::pair <std::string, bool>> bool_parameters;

private:
	ax::NodeEditor::EditorContext* ed_context = nullptr;

public:
	ResourceAnimatorController();
	~ResourceAnimatorController();

	std::string name = "New Animator Controller";

	//Parameters things
	std::vector < std::pair <std::string, bool>> GetBoolParameters();
	std::vector < std::pair <std::string, float>> GetFloatParameters();
	std::vector <std::pair <std::string, int>> GetIntParameters();
	void AddBoolParameter();
	void AddFloatParameter();
	void AddIntParameter();
	void SetBool(std::string name, bool value);
	void SetFloat(std::string name, float value);
	void SetInt(std::string name, int value);

public:

/*	virtual void FreeMemory();
	virtual bool LoadMemory()*/;
	//AnimationHandle
	void Play();
	void Play(std::string state_name);
	void Update();
	void UpdateState(State* state);
	void Stop();
	std::vector<bool> GetTriggers() const { return triggers; }
	void CheckTriggers();

	//void SaveAsset();

	//Transform
	bool GetTransform(std::string channel_name, float3& position, Quat& rotation, float3& scale);
	bool GetTransformState(State* state, std::string channel_name, float3& position, Quat& rotation, float3& scale);

	//States
	void AddState(std::string name, ResourceAnimation* clip, float speed = 1.0F);
	void RemoveState(std::string name);
	State* FindState(std::string name);
	State* FindState(uint id);
	State* FindStateFromPinId(uint pin_id);

	uint GetNumStates() const { return states.size(); }
	std::vector<State*> GetStates() { return states; }

	//Transitions
	void AddTransition(State* source, State* target, float blend);
	void AddTransition(State* source, State* target, float blend, uint trigger);
	void RemoveTransition(std::string source_name, std::string target_name);
	std::vector<Transition*> GetTransitions() const { return transitions; }
	uint GetNumTransitions() const { return transitions.size(); }
	std::vector<Transition*> FindTransitionsFromSourceState(State* state);

	State* GetDefaultNode() const { return default_state; };

	ax::NodeEditor::EditorContext* GetEditorContext();
	std::string GetTypeString() const;

	std::string GetName();

	//void UnLoad();
	//void Load();
	void Reset();

	friend class Time;
};

#endif // !__ANIMATOR_CONTROLLER_H_
