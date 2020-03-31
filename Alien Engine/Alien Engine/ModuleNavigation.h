#ifndef _MODULE_NAVIGATION_H__
#define _MODULE_NAVIGATION_H__

#include "Module.h"
//#include <list>



class GameObject;

class ModuleNavigation : public Module
{
	friend class ModuleObjects;
	friend class PanelNavigation;

public:
	ModuleNavigation(bool start_enabled = true);

	~ModuleNavigation();

	void LoadConfig(JSONfilepack*& config);
	void SaveConfig(JSONfilepack*& config);
	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:
	bool Bake(std::list<GameObject*> go_meshes);

public:
	bool show_navmesh = true;
	bool show_heightmesh = false;
	
private:
	std::list<uint64_t> go_meshes_id;
};


#endif // !_MODULE_NAVIGATION_H__

