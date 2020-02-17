#include "ComponentText.h"

ComponentText::ComponentText(GameObject* obj): ComponentUI(obj)
{
}

bool ComponentText::DrawInspector()
{
	return true;
}
