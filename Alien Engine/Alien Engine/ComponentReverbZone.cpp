#include "Application.h"
#include "ComponentAudioEmitter.h"
#include "ComponentTransform.h"
#include "ComponentReverbZone.h"
#include "ModuleAudio.h"
#include "mmgr/mmgr.h"

#define MAX(a,b) ((a)>(b)) ? (a) : (b)

ComponentReverbZone::ComponentReverbZone(GameObject * parent) : Component(parent)
{
	type = ComponentType::A_REVERB;
}

ComponentReverbZone::~ComponentReverbZone()
{

}

void ComponentReverbZone::Update()
{
	/*sphere.pos = game_object_attached->GetComponentTransform()->position;
	AABB box = sphere.MinimalEnclosingAABB();
	OBB boundingBox(box);
	box.Transform(parent->GetComponentTransform()->GetGlobalMatrix());

	DebugDrawSphere();
	for (int i = 0; i < App->scene_intro->root->children.size(); ++i) {
		ComponentAudioEmitter* audio_emitter = App->scene_intro->root->children[i]->GetComponentAudioEmitter();

		if (audio_emitter != nullptr && App->scene_intro->root->children.size() >= 0)
		{
			for (int j = 0; j < App->scene_intro->root->children[i]->children.size(); ++j) {
				if (sphere.Intersects(App->scene_intro->root->children[i]->children[j]->bounding_box) == true) {
					audio_emitter->source->ApplyEnvReverb(1.0f, "Rain");
				}
				else {
					audio_emitter->source->ApplyEnvReverb(0.0f, "Rain");
				}
			}
		}
	}*/
}

void ComponentReverbZone::SaveReverbZone(JSON_Array * componentsObj) const
{
	/*JSON_Value* component = json_value_init_object();
	JSON_Object* componentObj = json_value_get_object(component);
	json_object_set_number(componentObj, "Type:", this->type);
	json_object_set_number(componentObj, "Radius:", this->radius);

	json_array_append_value(componentsObj, component);*/
}

void ComponentReverbZone::LoadReverbZone(JSON_Object * obj, GameObject * go)
{
	/*App->scene_intro->gameobject_scene->reverb_zone == nullptr;
	App->scene_intro->GOPath->reverb_zone == nullptr;
	go->reverb_zone->radius = json_object_get_number(obj, "Radius:");*/
}
