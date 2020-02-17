#ifndef _COMPONENT_UI_H_
#define _COMPONENT_UI_H_

#include "Component.h"
#include "ComponentCanvas.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float2.h"
#include "Color.h"

class ResourceTexture;

enum UIState {
	Idle,
	Hover,
	Click,
	Pressed,
	Release
};

class __declspec(dllexport) ComponentUI :public Component {
	friend class GameObject;
	friend class ComponentImage;
	friend class ComponentCheckbox;
	friend class Component;
	friend class ReturnZ;
	friend class CompZ;
public:
	ComponentUI(GameObject* obj);

	virtual ~ComponentUI();

	virtual bool OnHover() { return true; };
	virtual bool OnClick() { return true; };
	virtual bool OnPressed() { return true; };
	virtual bool OnRelease() { return true; };

	void SetCanvas(ComponentCanvas* canvas_);

	void Update();
	virtual void Draw(bool isGame);

	void ClearTexture();
	void SetTexture(ResourceTexture* tex);

private:
	bool CheckMouseInside(float3 mouse_pos);
	void UILogic();

protected:
	int x = 0, y = 0;

	ComponentType ui_type = ComponentType::UI;
	UIState state = Idle;

	ResourceTexture* texture = nullptr;

	float3 vertices[4]{ {-1,1,0},{-1,-1,0},{1,-1,0},{1,1,0} };  // left-top, left-bottom, right-bottom, right-top
	float2 uv[4]{ {-1,-1},{-1,0},{0,0},{0,-1} };
	uint index[6]{ 0,1,2,2,3,0 };
	uint indexID = 0;
	uint verticesID = 0;
	uint uvID = 0;

	Color current_color = Color::White();
	ComponentCanvas* canvas = nullptr;
	ComponentTransform* canvas_trans = nullptr;

};


#endif