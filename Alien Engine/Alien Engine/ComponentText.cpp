#include "Application.h"
#include "ModuleResources.h"
#include "ModuleObjects.h"
#include "ComponentTransform.h"
#include "ComponentText.h"
#include "ResourceFont.h"
#include "ResourceShader.h"
#include "Viewport.h"
#include "ReturnZ.h"
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

	GenerateVAOVBO();

	ui_type = ComponentType::UI_TEXT;
	tabbable = false;
}

bool ComponentText::DrawInspector()
{
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Text", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Text");

		ImGui::Spacing();

		static char text_str[MAX_PATH];
		memcpy(text_str, text.data(), MAX_PATH);

		ImGui::Text("Text: "); ImGui::SameLine();
		if (ImGui::InputText("##Text", text_str, MAX_PATH, ImGuiInputTextFlags_AutoSelectAll)) {
			text = text_str;
		}

		static bool set_bg_Z = true;
		static Color bg_col;
		if (ImGui::ColorEdit4("##Color", &bg_col, ImGuiColorEditFlags_Float)) {
			if (set_bg_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_bg_Z = false;
			current_color = bg_col;
		}
		else if (!set_bg_Z && ImGui::IsMouseReleased(0)) {
			set_bg_Z = true;
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Text");
	}
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
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Activate Shader
	canvas->text_shader->Bind();
	canvas->text_shader->SetUniformFloat3("textColor", float3(current_color.r, current_color.g, current_color.b));
	canvas->text_shader->SetUniformMat4f("projection", App->renderer3D->scene_fake_camera->GetProjectionMatrix4f4());
	canvas->text_shader->SetUniformMat4f("view", App->renderer3D->scene_fake_camera->GetViewMatrix4x4());
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	
	std::string::const_iterator c;
	x = 0;
	for(c = text.begin(); c != text.end(); c++) {
		Character ch = font->fontData.charactersMap[*c];

		float xpos = x + ch.bearing.x;
		float ypos = (ch.size.y - ch.bearing.y);
		float w = ch.size.x * 0.25;
		float h = ch.size.y * 0.25;

		float vertex[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += ch.advance;
	}
	
	canvas->text_shader->Unbind();
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
}

void ComponentText::Clone(Component* clone)
{
	clone->enabled = enabled;
	ComponentUI* ui = (ComponentUI*)clone;
	ui->current_color = current_color;

	dynamic_cast<ComponentText*>(clone)->text = text;
	if (font != nullptr) {
		dynamic_cast<ComponentText*>(clone)->font = font;
	}
	else {
		LOG_ENGINE("There's no default font!");
	}

	GameObject* p = game_object_attached->parent;
	bool changed = true;
	while (changed) {
		if (p != nullptr) {
			ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
			if (canvas != nullptr) {
				ui->SetCanvas(canvas);
				changed = false;
			}
			p = p->parent;
		}
		else {
			changed = false;
			ui->SetCanvas(nullptr);
		}
	}
}

void ComponentText::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetString("Text", text);
	to_save->SetNumber("UIType", (int)ui_type);
	if (font != nullptr) {
		to_save->SetString("FontID", std::to_string(font->GetID()));
	}
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetColor("Color", current_color);
}

void ComponentText::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	text = to_load->GetString("Text");

	u64 fontID = std::stoull(to_load->GetString("FontID"));
	if (fontID != 0) {
		ResourceFont* font = (ResourceFont*)App->resources->GetResourceWithID(fontID);
	}
	else {
		LOG_ENGINE("Font ID equals to 0! Font not founded");
	}

	GameObject* p = game_object_attached->parent;
	bool changed = true;
	while (changed) {
		if (p != nullptr) {
			ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
			if (canvas != nullptr) {
				SetCanvas(canvas);
				changed = false;
			}
			p = p->parent;
		}
		else {
			changed = false;
			SetCanvas(nullptr);
		}
	}
}

void ComponentText::SetFont(ResourceFont* font)
{
	this->font = font;
}

ResourceFont* ComponentText::GetFont() const
{
	return font;
}

void ComponentText::GenerateVAOVBO()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}




