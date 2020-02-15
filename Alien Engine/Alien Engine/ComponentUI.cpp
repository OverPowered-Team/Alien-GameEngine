#include "Component.h"
#include "ComponentUI.h"
#include "Application.h"
#include "ModuleUI.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentTransform.h"
#include "ResourceTexture.h"
#include "PanelGame.h"

ComponentUI::ComponentUI(GameObject* obj):Component(obj)
{
	x = 0;
	y = 0;
	width = 40;
	height = 20;
	
	glGenBuffers(1, &verticesID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &uvID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uvID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 2, uv, GL_STATIC_DRAW);

	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 6 * 3, index, GL_STATIC_DRAW);

	type = ComponentType::UI;
}

ComponentUI::~ComponentUI()
{
	glDeleteBuffers(1, &uvID);
	glDeleteBuffers(1, &verticesID);
	glDeleteBuffers(1, &indexID);

	if (texture != nullptr) {
		texture->DecreaseReferences();
	}
}

void ComponentUI::SetCanvas(ComponentCanvas* canvas_)
{
	if (canvas_ != nullptr) {
		canvas = canvas_;
		canvas_trans = canvas->game_object_attached->GetComponent<ComponentTransform>();
	}
	else {
		canvas = nullptr;
		canvas_trans = nullptr;
	}
}

void ComponentUI::Update()
{
	UILogic();

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
	
	glDisable(GL_CULL_FACE);
	
	if (texture != nullptr) {
		glAlphaFunc(GL_GREATER, 0.0f);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}

	glColor4f(current_color.r, current_color.g, current_color.b, current_color.a);

	if (transform->IsScaleNegative())
		glFrontFace(GL_CW);

	glPushMatrix();
	glMultMatrixf(transform->global_transformation.Transposed().ptr());

	glEnableClientState(GL_VERTEX_ARRAY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glDrawElements(GL_TRIANGLES, 6 * 3, GL_UNSIGNED_INT, 0);

	if (transform->IsScaleNegative())
		glFrontFace(GL_CCW);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	glEnable(GL_CULL_FACE);
}

bool ComponentUI::CheckMouseInside(float3 mouse_pos)
{
	return (mouse_pos.x >= x && mouse_pos.x <= x + width && mouse_pos.y >= y && mouse_pos.y <= y + height);
}

void ComponentUI::UILogic()
{
	float3 mouse_pos;

#ifndef GAME_VERSION
	mouse_pos = float3((App->input->GetMouseX() - App->ui->panel_game->posX), (App->input->GetMouseY() - App->ui->panel_game->posY) - 19, App->input->GetMouseZ());
#else
	mouse_pos = App->input->GetMousePosition();
#endif

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
