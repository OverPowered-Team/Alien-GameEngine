#pragma once

#include <string>

class GameObject;
struct AlienEvent;

typedef unsigned int uint;
class JSONArraypack;
typedef unsigned long long u64;

enum class ComponentType {
	NONE = -1,
	TRANSFORM = 0,
	MESH ,
	MATERIAL ,
	LIGHT_DIRECTIONAL ,
	LIGHT_SPOT ,
	LIGHT_POINT ,
	CAMERA ,

	BOX_COLLIDER ,
	SPHERE_COLLIDER ,
	CAPSULE_COLLIDER ,
	MESH_COLLIDER ,
	CONVEX_HULL_COLLIDER ,
	HEIGHTFIELD_COLLIDER ,
	RIGID_BODY ,
	POINT_CONSTRAINT ,
	CHARACTER_CONTROLLER ,

	ANIMATOR ,
	PARTICLES ,
	A_EMITTER ,
	A_LISTENER ,
	CANVAS ,
	UI_IMAGE ,
	UI_BUTTON ,
	UI_TEXT ,
	UI_CHECKBOX ,
	UI_SLIDER ,
	UI_BAR ,
	UI_ANIMATED_IMAGE ,
	DEFORMABLE_MESH ,
	BONE ,
	SCRIPT ,
	UI ,
	CURVE,

	MAX //LAST LAST LAST
};

class __declspec(dllexport) Component {
	friend class ComponentCamera;
	friend class ComponentLightDirectional;
	friend class ComponentLightSpot;
	friend class ComponentLightPoint;
	friend class ComponentMaterial;
	friend class ComponentTransform;
	friend class ComponentMesh;
	friend class ComponentDeformableMesh;
	friend class ComponentMaterial;
	friend class ComponentCanvas;
	friend class ComponentUI;
	friend class ComponentPhysics;
	friend class ComponentCharacterController;
	friend class ComponentRigidBody;
	friend class ComponentCollider;
	friend class ComponentScript;
	friend class ComponentImage;
	friend class ComponentText;
	friend class GameObject;
	friend class ReturnZ;
	friend class Prefab;
	friend class CompZ;
	friend class PanelInspector;
	friend class ModuleObjects;
	friend class ModulePhysX;
	friend class ModuleUI;
public:
	Component(GameObject* attach);
	virtual ~Component();

	bool IsEnabled();
	void SetEnable(bool enable);
	void Destroy();

public:
	GameObject* game_object_attached = nullptr;

protected:

	virtual void Reset() {}
	virtual void SetComponent(Component* component) {}
	virtual void Clone(Component* clone) {}

	virtual void SaveComponent(JSONArraypack* to_save) {}
	virtual void LoadComponent(JSONArraypack* to_load) {}

	virtual void OnPlay() {}
	virtual void OnPause() {}
	virtual void OnStop() {}

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}

	virtual void DrawScene() {}
	virtual void DrawGame() {}

	virtual void ResetIDs();

	const ComponentType& GetType() const;

	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void HandleAlienEvent(const AlienEvent& alien_event) {};

	virtual bool DrawInspector() { return true; }

	static std::string EnumToString(ComponentType type);

protected:

	void RightClickMenu(const char* collapsing_header_name);

protected:

	ComponentType type = ComponentType::NONE;
	bool enabled = true;
	u64 ID = 0;
	bool not_destroy = true;
	const char* name = "";
	bool serialize = true;
};
