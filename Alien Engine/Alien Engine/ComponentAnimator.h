#pragma once
#ifndef _COMPONENT_ANIMATOR_H_
#define _COMPONENT_ANIMATOR_H_

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
	void OnPlay();

	ResourceAnimatorController* GetResourceAnimatorController();
	void SetAnimatorController(ResourceAnimatorController* controller);

	//bool Save(const nlohmann::json::iterator& it);
	//bool Load(const nlohmann::json comp);

private:
	ResourceAnimatorController* animator_controller;

};

#endif // !_COMPONENT_ANIMATOR_H_

