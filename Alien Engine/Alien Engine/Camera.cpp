#include "Camera.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleObjects.h"
#include "Viewport.h"

const std::vector<ComponentCamera*>& Camera::GetAllCameras()
{
	return App->objects->game_cameras;
}

ComponentCamera* Camera::GetCurrentCamera()
{
	return App->renderer3D->actual_game_camera;
}

void Camera::SetCurrentCamera(ComponentCamera* camera)
{
	if (camera != nullptr) {
		App->renderer3D->actual_game_camera = camera;
		App->objects->game_viewport->SetCamera(camera);
	}
	else {
		LOG_ENGINE("Camera was nullptr in Camera::SetCurrentCamera");
	}
}

uint Camera::GetNumCameras()
{
	return App->objects->game_cameras.size();
}
