#include "ComponentCanvas.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "glew/include/glew.h"


ComponentCanvas::ComponentCanvas(GameObject* obj, uint w, uint h):Component(obj)
{
	width = 160;
	height = 90;

	type = ComponentType::CANVAS;
}

void ComponentCanvas::Draw()
{
	ComponentTransform* comp_trans = game_object_attached->GetComponent<ComponentTransform>();

	glBegin(GL_LINE_LOOP);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	float3 pos = comp_trans->GetGlobalPosition();

	float3 v1 = float3(pos.x - width * 0.5, pos.y + height * 0.5, pos.z);
	float3 v2 = float3(pos.x + width * 0.5, pos.y + height * 0.5, pos.z);
	float3 v3 = float3(pos.x - width * 0.5, pos.y - height * 0.5, pos.z);
	float3 v4 = float3(pos.x + width * 0.5, pos.y - height * 0.5, pos.z);

	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v4.x, v4.y, v4.z);
	glVertex3f(v3.x, v3.y, v3.z);

	glEnd();
}
