#include "TestMove.h"

TestMove::TestMove() : Alien()
{
}

TestMove::~TestMove()
{
}

void TestMove::Update()
{
	
	X = Input::GetControllerHoritzontalLeftAxis(controllerNum);
	Y = Input::GetControllerVerticalLeftAxis(controllerNum);
	if (Input::GetControllerButtonRepeat(controllerNum, Input::CONTROLLER_BUTTONS::CONTROLLER_BUTTON_A)) {
		X = 1;
	}
	if (Input::GetControllerButtonRepeat(controllerNum, Input::CONTROLLER_BUTTONS::CONTROLLER_BUTTON_Y)) {
		X = -1;
	}
	if (Input::GetControllerButtonRepeat(controllerNum, Input::CONTROLLER_BUTTONS::CONTROLLER_BUTTON_DPAD_DOWN)) {
		Y = -1;
	}
	if (Input::GetControllerButtonRepeat(controllerNum, Input::CONTROLLER_BUTTONS::CONTROLLER_BUTTON_LEFTTRIGGER)) {
		Y = 1;
	}
	if (Input::GetControllerButtonDown(controllerNum, Input::CONTROLLER_BUTTONS::CONTROLLER_BUTTON_LEFTSTICK)) {
		Input::DoRumble(controllerNum, 0.8, 2000);
	}
	transform->SetLocalPosition(transform->GetLocalPosition() + float3{velocity * Time::GetDT() * X, 0, velocity * Time::GetDT() * Y });

}
