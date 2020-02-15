#ifndef _COMPONENT_UI_H_
#define _COMPONENT_UI_H_

#include "Component.h"
#include "ComponentCanvas.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float2.h"

struct UIPanel {
	float3 vertex[4];
	float2 uv[4];
	uint buffer[3];
	uint index[6] = { 0,1,2,2,1,3 };

	uint textureID;

	void GenerateBuffer();
};

enum UIState {
	Idle,
	Hover,
	Click,
	Clicked,
	Release
};

class ComponentUI :public Component {
public:
	ComponentUI(GameObject* obj, ComponentCanvas* canvas_);

	virtual ~ComponentUI() {};

	virtual bool OnHover() { return true; };
	virtual bool OnClick() { return true; };
	virtual bool OnClicked() { return true; };
	virtual bool OnRelease() { return true; };

	void Update();
	virtual void Draw();

private:
	bool CheckMouseInside(float3 mouse_pos);
	void CreatePanel();
	void UILogic();

protected:
	int x = 0, y = 0, width = 0, height = 0;
	bool draggable = false;
	UIState state = Idle;
	UIPanel panel;

	ComponentCanvas* canvas = nullptr;
	ComponentTransform* canvas_trans = nullptr;
};


#endif