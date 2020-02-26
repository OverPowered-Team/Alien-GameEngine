#pragma once
#include "Component.h"
#include <string>
class ResourceAnimatorController;


class __declspec(dllexport) ComponentAnimator : public Component
{
	friend class ReturnZ;
	friend class CompZ;
	friend class GameObject;
	friend class ModuleImporter;
public:
	ComponentAnimator(GameObject* gameobject);
	virtual ~ComponentAnimator();
	void Update();

	void PlayState(std::string name);
	void UpdateAnimation(GameObject* go_to_update);
	void OnPlay();

	ResourceAnimatorController* GetResourceAnimatorController();
	void SetAnimatorController(ResourceAnimatorController* controller);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

private:
	ResourceAnimatorController* animator_controller;

	bool DrawInspector();

};


