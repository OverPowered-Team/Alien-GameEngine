#include "player_example.h"

player_example::player_example() : Alien()
{
}

player_example::~player_example()
{
}

void player_example::Start()
{
	ccontroller = GetComponent<ComponentCharacterController>();
	moveDirection = float3::zero();
}

void player_example::Update()
{
    // * ---------------- OVERLAP ----------------- * //
    float overlap_r = 6.0f;
    const std::vector<ComponentCollider*> colliders =  Physics::OverlapSphere(transform->GetGlobalPosition(), overlap_r);

    for (uint i = 0; i < colliders.size(); ++i)
    {
        //LOG("Overlapping sphere on r: %f gameobject: %s", overlap_r, colliders[i]->game_object_attached->GetName());
    }

	// * ------------------------- RAYCAST --------------------------------* //
	// FIRST CONTACT
	//PxRaycastBuffer hit;
	//float3 raypos = transform->GetGlobalPosition();
	//bool anyHit = Physics::Raycast(float3(raypos.x, raypos.y, raypos.z + 3.0f), transform->forward.Normalized(), 5.0f, hit);
	//if (anyHit)
	//{
	//	//LOG_ENGINE("num touches %i", hit.getNbTouches());
	//	ComponentCollider* col = (ComponentCollider*)hit.block.shape->userData;
	//	if (col)
	//		LOG_ENGINE("raycast hit with %s", col->game_object_attached->GetName());
	//}

	// ALL CONTACTS
	// TODO: TEST WHEN WE UPGRADE THIS BRANCH
	//Physics::RaycastAll(transform->GetGlobalPosition(), transform->forward.Normalized(), 10.0f);

	//* -------------------------------------------------------------------* //

    // * ---------------------- SIMPLE CONTROLLER ------------------------ * //

	if (!ccontroller)
		return;

	float joystick_threshold = 0.2f;
	float x_axis = Input::GetControllerHoritzontalLeftAxis(1);
	float y_axis = Input::GetControllerVerticalLeftAxis(1);
	bool jump = Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_A) ||Input::GetKeyDown(SDL_SCANCODE_A);

	if (ccontroller->isGrounded)
	{
		// TODO: rework this shit, joystick values must be received already thresholded correctly, anybody says deadzone?
		moveDirection = float3(abs(x_axis) >= joystick_threshold ? -x_axis : 0.f, 0.f, abs(y_axis) >= joystick_threshold ? -y_axis : 0.f);
		moveDirection *= speed;

		if (jump)
			moveDirection.y = 10.0f;
	}

	//* if force_gravity are unchecked on character controller (recommended) *//
	//* but works correctly though*//
	//moveDirection.y -= gravity * Time::GetDT();

	ccontroller->Move(moveDirection * Time::GetDT());

	//* -------------------------------------------------------------------* //


}

void player_example::OnControllerColliderHit(ControllerColliderHit hit)
{
    LOG("OnControllerColliderHit: %s", hit.gameObject->GetName());
}

void player_example::OnCollisionEnter(const Collision& collision)
{
    LOG("OnCollisionEnter: %s", collision.game_object->GetName());
}

void player_example::OnCollisionStay(const Collision& collision)
{
	LOG("OnCollisionStay: %s", collision.game_object->GetName());
}

void player_example::OnCollisionExit(const Collision& collision)
{
	LOG("OnCollisionExit: %s", collision.game_object->GetName());
}

void player_example::OnTriggerEnter(ComponentCollider* collider)
{
    LOG("OnTriggerEnter: %s", collider->game_object_attached->GetName());
}