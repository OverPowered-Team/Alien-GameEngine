#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <vector>
#include <string>

#include "Globals.h"

class Skybox
{
public:
	Skybox();
	~Skybox();

	uint LoadCubeMap(const std::vector<std::string>& texture_files);

public:
	std::vector<std::string> textures[6];
};

#endif /* __SKYBOX_H__ */