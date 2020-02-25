#pragma once

#include "Component.h"
class JSONArraypack;
class ParticleSystem;

class __declspec(dllexport) ComponentParticleSystem : public Component {
	friend class ReturnZ;
	friend class CompZ;
	friend class GameObject;
	friend class ModuleObjects;
	friend class PanelCreateObject;
	friend class ModuleImporter;

public: 

	ComponentParticleSystem(GameObject* parent);
	~ComponentParticleSystem();

private:

	void OnPlay() override; 

	void PreUpdate() override; 
	void Update() override;
	void PostUpdate() override;

	void DebugDraw();
	void Draw();

	void OnEnable() override;
	void OnDisable() override;

	bool DrawInspector () override;

	void TextureBrowser();

	// Serialization Component
	void SaveComponent(JSONArraypack* to_save) override;
	void LoadComponent(JSONArraypack* to_load) override;

	// Serialization File
	void LoadParticles();
	void SaveParticles();

public: 

	ParticleSystem* GetSystem();
	void SetTexture(ResourceTexture* tex);

	void Play();
	void Pause();
	void Restart(); 

private:

	ParticleSystem* particleSystem = nullptr;
	bool drawEmmitter = false;
	int shapeSelected = 0;
	int bbTypeSelected = 0;
	int matTypeSelected = 0;
	int eqTypeSelected = 0;
	int funcTypeSource = 6;
	int funcTypeDest = 7;
	int transformSelected = 0; // 0 Global - 1 Local

	ResourceTexture* texture = nullptr;
	ResourceTexture* selected_texture = nullptr;

	bool change_texture_menu = false;
	bool texture_activated = true;

	bool enable_render = false;
};