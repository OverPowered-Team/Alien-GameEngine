#pragma once

#include "ModuleImporter.h"
#include "Component.h"
#include "Color.h"
#include "TextEdit/TextEditor.h"

class ResourceTexture;
class ResourceShader;

class __declspec(dllexport) ComponentMaterial : public Component {
	friend class ReturnZ;
	friend class CompZ;
	friend class GameObject;
	friend class ModuleImporter;
	friend class ResourceMesh;
public:
	ComponentMaterial(GameObject* attach);
	virtual ~ComponentMaterial();

private:
	bool DrawInspector();

	void InspectorShaderProperties();

	void Reset();
	void SetComponent(Component* component);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void Clone(Component* clone);

	const ResourceMaterial* GetMaterial() const;

public:
	void SetTexture(ResourceTexture* tex);
	void SetMaterial(ResourceMaterial* mat);

private:
	bool change_texture_menu = false;

// Shaders
public:
	void ShowShaderTextEditor();
	ResourceMaterial* material = nullptr; 

private:
	TextEditor shader_editor;
	TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::GLSL();
	std::string file_to_edit;
	bool show_shader_text_editor = false;
	bool select_shader = false;
};