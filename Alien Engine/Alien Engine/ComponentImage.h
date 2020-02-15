#ifndef _COMPONENT_IMAGE_H_
#define _COMPONENT_IMAGE_H_

#include "ComponentUI.h"

class ComponentImage :public ComponentUI
{
public:
	ComponentImage(GameObject* obj, ComponentCanvas* canvas_);
	~ComponentImage() {};

};

#endif // !_COMPONENT_IMAGE_H_
