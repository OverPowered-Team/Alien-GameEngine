#ifndef __MODULEAUDIO_H__
#define __MODULEAUDIO_H__

#include "Module.h"
#include "Globals.h"
#include "WwiseT.h"
#include "ComponentAudioEmitter.h"

#include <list>

class ModuleAudio : public Module
{
public:
	ModuleAudio();
	~ModuleAudio();
	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	// Utils
	void Play();
	void Stop();
	void Pause()const;
	void Resume()const;
	void SetListener(WwiseT::AudioSource* new_listener);
	WwiseT::AudioSource* CreateSoundEmitter(const char * name);
private:
	//std::list<WwiseT::AudioSource*> event_list;
	WwiseT::AudioSource* listener;
	WwiseT::AudioSource* source;
public:
	std::list<AudioEmitter*> audios;
	
	bool is_playing = false;
};

#endif // __ModuleAudio_H__