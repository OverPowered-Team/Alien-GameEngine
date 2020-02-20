#include "ComponentAudioListener.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ComponentTransform.h"
#include "MathGeoLib\include\MathGeoLib.h"

AudioListener::AudioListener(GameObject * parent) : Component(parent)
{
	listener = App->audio->CreateSoundEmitter("listener"); //CREATING LISTENER SAME AS AN EMITTER?
	App->audio->SetListener(listener);
}

AudioListener::~AudioListener()
{
	//RELEASE(listener);
}

void AudioListener::Update(float dt)
{

}

void AudioListener::UpdateListenerPos()
{
	
}

void AudioListener::SaveListener(/*JSON_Array* componentsObj*/) const
{
	/*JSON_Value* component = json_value_init_object();
	JSON_Object* componentObj = json_value_get_object(component);
	json_object_set_number(componentObj, "Type:", this->type);

	json_array_append_value(componentsObj, component);*/
}