#include "PlayerController.h"

PlayerController::PlayerController() : Alien()
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Start()
{
	animator = (ComponentAnimator*)GetComponent(ComponentType::ANIMATOR);
	ccontroller = (ComponentCharacterController*)GetComponent(ComponentType::CHARACTER_CONTROLLER);
}

void PlayerController::Update()
{

	/*---------------CONTROLLER-----------------------*/
	HandleMovement();

	/*---------------CONTROLLER-----------------------*/

	switch (state)
	{
	case PlayerController::PlayerState::IDLE: {

		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_X)) {
			animator->PlayState("Attack");
			state = PlayerState::BASIC_ATTACK;
		}

		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_RIGHTSHOULDER)) {
			animator->PlayState("Roll");
			state = PlayerState::DASHING;
		}

		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_A)) {
			/*animator->PlayState("Roll");*/
			ccontroller->SetFallSpeed(playerData.fallSpeed);
			ccontroller->SetJumpSpeed(playerData.jumpSpeed);
			ccontroller->SetMaxJumpHeight(playerData.maxHeight);
			state = PlayerState::JUMPING;
			if(ccontroller->CanJump())ccontroller->Jump();
		}

	} break;
	case PlayerController::PlayerState::RUNNING:
	{
		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_X)) {
			animator->PlayState("Attack");
			state = PlayerState::BASIC_ATTACK;
		}

		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_RIGHTSHOULDER)) {
			animator->PlayState("Roll");
			state = PlayerState::DASHING;
		}

		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_A)) {
			/*animator->PlayState("Roll");*/
			state = PlayerState::JUMPING;
			ccontroller->SetFallSpeed(playerData.fallSpeed);
			ccontroller->SetJumpSpeed(playerData.jumpSpeed);
			ccontroller->SetMaxJumpHeight(playerData.maxHeight);
			if (ccontroller->CanJump())ccontroller->Jump();
		}

	} break;
	case PlayerController::PlayerState::BASIC_ATTACK:
		break;
	case PlayerController::PlayerState::JUMPING:
		break;
	case PlayerController::PlayerState::DASHING:
		break;
	case PlayerController::PlayerState::MAX:
		break;
	default:
		break;
	}

	/*---------------KEYBOARD-----------------------*/

	if (state == PlayerState::RUNNING && abs(playerData.currentSpeed) < 0.1F)
		state = PlayerState::IDLE;

	if (state == PlayerState::IDLE && abs(playerData.currentSpeed) > 0.1F)
		state = PlayerState::RUNNING;

	if (state == PlayerState::JUMPING && ccontroller->CanJump()) {
		if(abs(playerData.currentSpeed) < 0.1F)
			state = PlayerState::IDLE;
		if (abs(playerData.currentSpeed) > 0.1F)
			state = PlayerState::RUNNING;
	}

	playerData.currentSpeed = 0;
}

void PlayerController::HandleMovement()
{
	float axisX = Input::GetControllerHoritzontalLeftAxis(controllerIndex);
	float axisY = Input::GetControllerVerticalLeftAxis(controllerIndex);

	float speed = (abs(axisX) + abs(axisY)) * 0.5F;

	float3 vector = float3(axisX, 0.f, axisY);
	vector = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalRotation().Mul(vector).Normalized();
	float angle = atan2f(vector.z, vector.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90) * Maths::Deg2Rad());

	if (abs(axisX) >= stick_threshold || abs(axisY) >= stick_threshold) {
		playerData.currentSpeed = (playerData.movementSpeed * speed * Time::GetDT());
		ccontroller->SetRotation(rot);
	}

	ccontroller->SetWalkDirection(vector.Normalized() * playerData.currentSpeed);
	animator->SetFloat("speed", Maths::Abs(playerData.currentSpeed));
}

void PlayerController::OnAnimationEnd(const char* name) {
	if (strcmp(name, "Attack") == 0)
		state = PlayerState::IDLE;

	if (strcmp(name, "Roll") == 0)
		state = PlayerState::IDLE;
}