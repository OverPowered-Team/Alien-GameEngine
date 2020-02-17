#include "Application.h"
#include "ModuleResources.h"
#include "ComponentText.h"
#include "ResourceFont.h"
#include "glew/include/glew.h"

ComponentText::ComponentText(GameObject* obj) : ComponentUI(obj)
{
	text = "Hola";
	font = App->resources->GetFontByName("Arialn");

	uv[0] = { 0,0 };
	uv[1] = { 0,1 };
	uv[2] = { 1,1 };
	uv[3] = { 1,0 };

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uvID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 2, uv, GL_STATIC_DRAW);

	if (font == nullptr)
	{
		font = ResourceFont::ImportFontBySize("Assets/Fonts/Arialn.ttf", 60);
	}

	ui_type = ComponentType::UI_TEXT;
}

bool ComponentText::DrawInspector()
{
	

	return true;
}

bool ComponentText::DrawCharacter()
{

	return false;
}

void ComponentText::Draw(bool isGame)
{
	glColor4f(1.0, 1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, font->fontData.charactersMap[80].textureID);

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

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);

	/*glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = font->fontData.charactersMap[*c];

		float xpos = 0 + ch.bearing.x;
		float ypos = 0 - (ch.size.y - ch.bearing.y);

		float w = ch.size.x;
		float h = ch.size.y;

		float vertex[6][4] = {
			{ xpos,	ypos + h,	0.0, 0.0 },
			{ xpos,	ypos,		0.0, 0.0 },
			{ xpos + w,	ypos,	0.0, 0.0 },

			{ xpos,		ypos + h,	0.0, 0.0 },
			{ xpos + w, ypos,		1.0, 1.0 },
			{ xpos + w, ypos + h,	1.0, 0.0 }
		};
		glBindTexture(GL_TEXTURE_2D, ch.textureID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.advance >> 6);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);*/
}


