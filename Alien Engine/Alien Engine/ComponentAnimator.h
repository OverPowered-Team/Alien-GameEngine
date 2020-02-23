#pragma once
#include "Component.h"
#include <string>
class ResourceAnimatorController;


class ComponentAnimator :
	public Component
{
public:
	ComponentAnimator(GameObject* gameobject);
	virtual ~ComponentAnimator();
	void Update();

	void PlayState(std::string name);
	void UpdateAnimation(GameObject* go_to_update);
	void Awake();

	ResourceAnimatorController* GetResourceAnimatorController();
	void SetAnimatorController(ResourceAnimatorController* controller);

	//bool save(const nlohmann::json::iterator& it);
	//bool load(const nlohmann::json comp);

private:
	ResourceAnimatorController* animator_controller;

	bool DrawInspector();

};


