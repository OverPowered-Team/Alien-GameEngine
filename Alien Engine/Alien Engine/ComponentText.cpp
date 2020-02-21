#include "Application.h"
#include "ModuleResources.h"
#include "ComponentTransform.h"
#include "ComponentText.h"
#include "ResourceFont.h"
#include "glew/include/glew.h"
#include "mmgr/mmgr.h"

ComponentText::ComponentText(GameObject* obj) : ComponentUI(obj)
{
	text = "Hola";
	font = App->resources->default_font;

	uv[0] = { 0,0 };
	uv[1] = { 0,1 };
	uv[2] = { 1,1 };
	uv[3] = { 1,0 };

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uvID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 2, uv, GL_STATIC_DRAW);

	ui_type = ComponentType::UI_TEXT;
}

bool ComponentText::DrawInspector()
{
	

	return true;
}

bool ComponentText::DrawCharacter(Character ch)
{
	glBindTexture(GL_TEXTURE_2D, ch.textureID);

	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

void ComponentText::Draw(bool isGame)
{
	glDisable(GL_CULL_FACE);

	glColor4f(0.5, 1.0, 1.0, 1.0);

	glPushMatrix();
	glMultMatrixf(game_object_attached->GetComponent<ComponentTransform>()->global_transformation.Transposed().ptr());

	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnableClientState(GL_VERTEX_ARRAY);

	std::string::const_iterator c;
	int i = 0;
	for(c = text.begin(); c != text.end(); c++) {
		Character ch = font->fontData.charactersMap[*c];
		float xpos = font->fontData.charactersMap[*(c - 1)].advance * i + ch.bearing.x;
		float ypos = (ch.size.y - ch.bearing.y);
		float w = ch.size.x;
		float h = ch.size.y;

		vertices[0] = { xpos, ypos + h, 0 };
		vertices[1] = { xpos, ypos, 0 };
		vertices[2] = { xpos + w, ypos, 0 };
		vertices[3] = { xpos + w, ypos + h, 0 };
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 3, vertices, GL_DYNAMIC_DRAW);

		DrawCharacter(ch);
		i++;
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	glEnable(GL_CULL_FACE);
}


