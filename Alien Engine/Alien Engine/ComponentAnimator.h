#pragma once
#include "Component.h"
#include <string>
class resourceanimatorcontroller;


class ComponentAnimator :
	public Component
{
public:
	ComponentAnimator(GameObject* gameobject);
	virtual ~ComponentAnimator();
	//void update();

	//void playstate(std::string name);
	//void updateanimation(GameObject* go_to_update);
	//void onplay();

	//resourceanimatorcontroller* getresourceanimatorcontroller();
	//void setanimatorcontroller(resourceanimatorcontroller* controller);

	//bool save(const nlohmann::json::iterator& it);
	//bool load(const nlohmann::json comp);

private:
	resourceanimatorcontroller* animator_controller;

};


