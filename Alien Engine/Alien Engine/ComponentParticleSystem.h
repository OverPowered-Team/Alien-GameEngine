#pragma once

#include "Component.h"

class ParticleSystem;
class __declspec(dllexport) ComponentParticleSystem : public Component {
	friend class ReturnZ;
	friend class CompZ;
	friend class ResourceMesh;
	friend class ComponentLight;
	friend class ComponentMaterial;
	friend class ComponentTransform;
	friend class ComponentCamera;
	friend class GameObject;
	friend class ModuleCamera3D;
	friend class ModuleObjects;
	friend class Gizmos;
	friend class Octree;
	friend class OctreeNode;
	friend class PanelCreateObject;
	friend class PanelRender;

public:

	ComponentParticleSystem(GameObject* parent);
	~ComponentParticleSystem();

	void PreUpdate() override; 
	void Update() override;
	void PostUpdate() override;

	void DebugDraw();
	void Draw();

	void Reset() override;  

	void OnEnable() override;
	void OnDisable() override;

	bool DrawInspector () override;

	void TextureBrowser();
	void SetTexture(ResourceTexture* tex);
	ParticleSystem* GetSystem() const;

	// Parser
	void Save(JSONArraypack* to_save) const;
	void Load(JSONArraypack* to_load);

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