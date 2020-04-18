#include "player_controller_test.h"

player_controller_test::player_controller_test() : Alien()
{
}

player_controller_test::~player_controller_test()
{
}

void player_controller_test::Start()
{
	ccontroller = (ComponentCharacterController*)GetComponent(ComponentType::CHARACTER_CONTROLLER);
	moveDirection = float3::zero();
}

void player_controller_test::Update()
{
	if (!ccontroller)
		return;

	float joystick_threshold = 0.2f;
	float x_axis = Input::GetControllerHoritzontalLeftAxis(1);
	float y_axis = Input::GetControllerVerticalLeftAxis(1);
	bool jump = Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_A);
	
	if (ccontroller->isGrounded)
	{
		moveDirection = float3(abs(x_axis) >= joystick_threshold ? -x_axis : 0.f, 0.f, abs(y_axis) >= joystick_threshold ? -y_axis : 0.f);
		moveDirection *= speed;

		if (jump)
			moveDirection.y = 10.0f;
	}

	//* if force_gravity are unchecked on character controller (recommended) *//
	//* but works correctly though*//
	//moveDirection.y -= gravity * Time::GetDT();
	
	ccontroller->Move(moveDirection * Time::GetDT());
}

void player_controller_test::OnControllerColliderHit(const ControllerColliderHit& hit)
{
	if (hit.collider)
		LOG_ENGINE("Hit with object %s", hit.collider->game_object_attached->GetName());
}