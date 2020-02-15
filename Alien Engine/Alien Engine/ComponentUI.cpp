#include "Component.h"
#include "ComponentUI.h"
#include "Application.h"
#include "ModuleUI.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentTransform.h"
#include "ResourceTexture.h"
#include "PanelGame.h"

ComponentUI::ComponentUI(GameObject* obj, ComponentCanvas* canvas_):Component(obj)
{
	canvas = App->objects->GetRoot(true)->children.back()->GetComponent<ComponentCanvas>();
	canvas_trans = canvas->game_object_attached->GetComponent<ComponentTransform>();

	x = 0;
	y = 0;
	width = 40;
	height = 20;

	type = ComponentType::UI;
}

void ComponentUI::Update()
{
	UILogic();
	Draw();

	switch (state)
	{
	case Idle:
		break;
	case Hover:
		OnHover();
		break;
	case Click:
		OnClick();
		break;
	case Clicked:
		OnClicked();
		break;
	case Release:
		OnRelease();
		break;
	default:
		break;
	}
}

void ComponentUI::Draw()
{
	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);

	glPushMatrix();
	glMultMatrixf(transform->global_transformation.Transposed().ptr());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnableClientState(GL_VERTEX_ARRAY);

	if (panel.resourceTexture != nullptr) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, panel.resourceTexture->id);
	}

	glBindBuffer(GL_ARRAY_BUFFER, panel.buffer[0]);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, panel.buffer[1]);

	if (panel.resourceTexture != nullptr) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, panel.buffer[2]);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}

bool ComponentUI::CheckMouseInside(float3 mouse_pos)
{
	return (mouse_pos.x >= x && mouse_pos.x <= x + width && mouse_pos.y >= y && mouse_pos.y <= y + height);
}

void ComponentUI::CreatePanel()
{
	panel.vertex[0] = float3(0, 1, 0);
	panel.vertex[1] = float3(1, 1, 0);
	panel.vertex[3] = float3(1, 0, 0);
	panel.vertex[2] = float3(0, 0, 0);

	panel.uv[0] = float2(0, 1);
	panel.uv[1] = float2(1, 1);
	panel.uv[3] = float2(1, 0);
	panel.uv[2] = float2(0, 0);
}

void ComponentUI::UILogic()
{
	float3 mouse_pos;

#ifndef GAME_VERSION
	mouse_pos = float3((App->input->GetMouseX() - App->ui->panel_game->posX), (App->input->GetMouseY() - App->ui->panel_game->posY) - 19, App->input->GetMouseZ());
#else
	mouse_pos = App->input->GetMousePosition();
#endif

	LOG_ENGINE("x: %f, y:%f", mouse_pos.x, mouse_pos.y);

	switch (state)
	{
	case Idle:
		if (CheckMouseInside(mouse_pos))
			state = Hover;
		break;
	case Hover:
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			state = Click;
		if (!CheckMouseInside(mouse_pos))
			state = Release;
		break;
	case Click:
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			state = Clicked;
		break;
	case Clicked:
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && !CheckMouseInside(mouse_pos))
			state = Release;
		else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && CheckMouseInside(mouse_pos))
			state = Hover;
		break;
	case Release:
		state = Idle;
		break;

	}
}

void UIPanel::GenerateBuffer()
{
	//Cube Vertex
	glGenBuffers(1, &buffer[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * 4, vertex, GL_STATIC_DRAW);

	//Cube Vertex definition
	glGenBuffers(1, &buffer[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 6, index, GL_STATIC_DRAW);

	//UVs definition
	glGenBuffers(1, &buffer[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * 4, uv, GL_STATIC_DRAW);
}
