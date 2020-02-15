#include "ComponentImage.h"

ComponentImage::ComponentImage(GameObject* obj, ComponentCanvas* canvas_):ComponentUI(obj, canvas_)
{
	ui_type = ComponentType::UI_IMAGE;
}
