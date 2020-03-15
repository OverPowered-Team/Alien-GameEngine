#pragma once

#include "Component.h"
#include "Globals.h"

class GameObject;

class ComponentReverbZone : public Component
{
public:
	ComponentReverbZone(GameObject* parent);
	~ComponentReverbZone();

	void Update();

	void SaveReverbZone(JSON_Array* componentsObj) const; //TODO set save and load
	void LoadReverbZone(JSON_Object* obj, GameObject* go);

public:
	bool default = false;
	bool exclude_others = false;
	std::string aux_bus;
};
