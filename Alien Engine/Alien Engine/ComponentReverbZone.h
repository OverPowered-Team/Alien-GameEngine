#pragma once

#include "Component.h"
#include "Globals.h"
#include "MathGeoLib\include\MathGeoLib.h"

class GameObject;

class ReverbZone : public Component
{
public:
	ReverbZone(GameObject* parent);
	~ReverbZone();

	void Update(float dt);
	void SetRadius(float rad);
	void DebugDrawSphere();

	void SaveComponent(JSONArraypack* to_save) override;
	void LoadComponent(JSONArraypack* to_load) override;

public:
	float radius = 10.0f;

private:
	math::Sphere sphere;

};
