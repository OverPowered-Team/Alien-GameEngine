#ifndef __WATER_FRAME_BUFFERS_H__
#define __WATER_FRAME_BUFFERS_H__

#include "glew/include/glew.h"

class WaterFrameBuffers
{
public:
	WaterFrameBuffers();
	~WaterFrameBuffers();

public:
	static int reflection_width;
	static int reflection_height;

	static int refraction_width;
	static int refraction_height;

private:
	int reflection_frame_buffer;
	int reflectio_texture;
	int reflection_depth_buffer;

	int refraction_frame_buffer;
	int refraction_texture;
	int refraction_depth_texture;
};

#endif /* __WATER_FRAME_BUFFERS_H__ */