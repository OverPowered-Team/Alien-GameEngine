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

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

private:
	ResourceAnimatorController* animator_controller;

	bool DrawInspector();

};


