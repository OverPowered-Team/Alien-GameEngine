#ifndef _COMPONENT_CANVAS_H_
#define _COMPONENT_CANVAS_H_

#include "Component.h"

class GameObject;

class ComponentCanvas :public Component {
public:
	ComponentCanvas(GameObject* obj, uint w, uint h);
	~ComponentCanvas() {};

	void PreUpdate() {};
	void Update() {};
	void PostUpdate() {};

	void Draw();

public:
	uint width = 0, height = 0;
};


#endif // !_COMPONENT_CANVAS_H_