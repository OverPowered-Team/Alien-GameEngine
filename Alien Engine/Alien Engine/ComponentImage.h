#ifndef _COMPONENT_IMAGE_H_
#define _COMPONENT_IMAGE_H_

#include "ComponentUI.h"

class ResourceTexture;

class ComponentImage :public ComponentUI
{
public:
	ComponentImage(GameObject* obj);
	~ComponentImage() {};

	bool DrawInspector();
};

#endif // !_COMPONENT_IMAGE_H_
