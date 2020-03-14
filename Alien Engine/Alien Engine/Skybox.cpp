#include "glew/include/glew.h"
#include "stbi_image.h"

#include "Skybox.h"

Skybox::Skybox()
{

}

Skybox::~Skybox()
{

}

uint Skybox::LoadCubeMap(const std::vector<std::string>& texture_files)
{
	uint texture_id;
	glGenTextures(1, &texture_id);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
	
	int width, height, channels;
	unsigned char* data;
	for (int i = 0; i < texture_files.size(); ++i)
	{
		data = stbi_load(texture_files[i].c_str(), &width, &height, &channels, NULL);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			LOG_ENGINE("Cubemap texture couldn't be loaded: %s", texture_files[i].c_str());
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	return texture_id;
}
