#ifndef _COMPONENT_ANIMATED_IMAGE_H_
#define _COMPONENT_ANIMATED_IMAGE_H_

#include "ComponentUI.h"

class ResourceTexture;

class __declspec(dllexport) ComponentAnimatedImage :public ComponentUI
{
public:
	ComponentAnimatedImage(GameObject* obj);
	~ComponentAnimatedImage() {};

	bool DrawInspector();

	void Draw(bool isGame);

	void Reset() {};
	void SetComponent(Component* component) {};
	void Clone(Component* clone) {};

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	ResourceTexture* ClearTextureArray(ResourceTexture* item);
	ResourceTexture* SetTextureArray(ResourceTexture* tex, ResourceTexture* item);

private:
	std::vector<ResourceTexture*> images;
};

#endif // !_COMPONENT_ANIMATED_IMAGE_H_

