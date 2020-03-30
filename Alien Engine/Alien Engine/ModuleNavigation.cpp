#include "Globals.h"
#include "Application.h"
#include "ModuleNavigation.h"
#include "ModuleRenderer3D.h"
#include "Time.h"
#include "Alien.h"
#include "Optick/include/optick.h"

ModuleNavigation::ModuleNavigation(bool start_enabled) : Module(start_enabled)
{
}

ModuleNavigation::~ModuleNavigation()
{
	LOG_ENGINE("Destroying and cleaning module navigation");
	
}

void ModuleNavigation::LoadConfig(JSONfilepack*& config)
{

}
void ModuleNavigation::SaveConfig(JSONfilepack*& config)
{

}

bool ModuleNavigation::Init()
{
	OPTICK_EVENT();
	LOG_ENGINE("Init module navigation ");

	bool ret = true;
	

	return ret;
}

bool ModuleNavigation::Start()
{
	LOG_ENGINE("starting module navigation");

	return true;
}

// ---------------------------------------------------------
update_status ModuleNavigation::PreUpdate(float dt)
{
	OPTICK_EVENT();
	

	return UPDATE_CONTINUE;
}

update_status ModuleNavigation::Update(float dt)
{
	OPTICK_EVENT();


	return UPDATE_CONTINUE;
}

update_status ModuleNavigation::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleNavigation::CleanUp()
{
	return true;
}
// -----------------------------------------------------------

bool ModuleNavigation::Bake(std::list<GameObject*> go_meshes)
{
	bool ret = true;


	return ret;
}