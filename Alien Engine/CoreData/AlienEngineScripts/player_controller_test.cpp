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
	//* -------------------------- OVERLAP ----------------------------- * //
	// NEW APPROACH with same heap scripting
	/*std::vector<ComponentCollider*> colliders = Physics::OverlapSphere(transform->GetGlobalPosition(), 3.0f);

	for (uint i = 0; i < colliders.size(); ++i)
	{
		LOG_ENGINE("gameobject: %s", colliders[i]->game_object_attached->GetName());
	}*/

	// OLD APPROACH heap scripting
	/*ComponentCollider** colliders = nullptr;
	uint ncols = Physics::OverlapSphere(transform->GetGlobalPosition(), 3.0f, &colliders);

	for (int i = 0; i < ncols; ++i)
		LOG_ENGINE("gameobject: %s", colliders[i]->game_object_attached->GetName());*/

	//* -------------------------------------------------------------------* //

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
	if (hit.collider) // TODO: this is not needed when we release final system, actually needs because ground plane is "because yes object" and not throught gameobject/components pattern
	{
		// this script pushes all rigidbodies that the character touches

		LOG_ENGINE("Hit with object %s", hit.collider->game_object_attached->GetName());

		ComponentRigidBody* body = hit.rigidbody;
		float pushPower = 20.0f;

		if (body == nullptr || body->GetIsKinematic())
			return;

		float3 pushDir = float3(hit.moveDirection.x, 0.0f, hit.moveDirection.z);

		body->SetVelocity(pushDir * pushPower);
	}
}

void player_controller_test::OnCollisionEnter(const Collision& collision)
{
	LOG_ENGINE("OnCollisionEnter with object %s", collision.game_object->GetName());
}

void player_controller_test::OnTriggerEnter(ComponentCollider* collider)
{
	LOG_ENGINE("OnTriggerEnter with object %s", collider->game_object_attached->GetName());
}