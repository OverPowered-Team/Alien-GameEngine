#include "WaterFrameBuffers.h"

int WaterFrameBuffers::reflection_width = 320;
int WaterFrameBuffers::reflection_height = 180;

int WaterFrameBuffers::refraction_width = 1280;
int WaterFrameBuffers::refraction_height = 720;

WaterFrameBuffers::WaterFrameBuffers()
{
	InitialiseReflectionFrameBuffer();
	InitialiseRefractionFrameBuffer();
}

WaterFrameBuffers::~WaterFrameBuffers()
{
	glDeleteFramebuffers(1, &reflection_frame_buffer);
	glDeleteTextures(1, &reflection_texture);
	glDeleteRenderbuffers(1, &reflection_depth_buffer);
	glDeleteFramebuffers(1, &refraction_frame_buffer);
	glDeleteTextures(1, &refraction_texture);
	glDeleteTextures(1, &refraction_depth_texture);
}

void WaterFrameBuffers::BindReflectionFrameBuffer()
{
	BindFrameBuffer(reflection_frame_buffer, WaterFrameBuffers::reflection_width, WaterFrameBuffers::reflection_height);
}

void WaterFrameBuffers::BindRefractionFrameBuffer()
{
	BindFrameBuffer(refraction_frame_buffer, WaterFrameBuffers::refraction_width, WaterFrameBuffers::refraction_height);
}

void WaterFrameBuffers::UnbindCurrentFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, width, height);
}

uint WaterFrameBuffers::GetReflectionTexture()
{
	return reflection_texture;
}

uint WaterFrameBuffers::GetRefractionTexture()
{
	return refraction_texture;
}

uint WaterFrameBuffers::GetRefractionDepthTexture()
{
	return refraction_depth_texture;
}

void WaterFrameBuffers::InitialiseReflectionFrameBuffer()
{
	reflection_frame_buffer = CreateFrameBuffer();
	reflection_texture = CreateTextureAttachment(WaterFrameBuffers::reflection_width, WaterFrameBuffers::reflection_height);
	reflection_depth_buffer = CreateDepthBufferAttachment(WaterFrameBuffers::reflection_width, WaterFrameBuffers::reflection_height);
	UnbindCurrentFrameBuffer();
}

void WaterFrameBuffers::InitialiseRefractionFrameBuffer()
{
	refraction_frame_buffer = CreateFrameBuffer();
	refraction_texture = CreateTextureAttachment(WaterFrameBuffers::refraction_width, WaterFrameBuffers::refraction_height);
	refraction_depth_texture = CreateDepthTextureAttachment(WaterFrameBuffers::refraction_width, WaterFrameBuffers::refraction_height);
	UnbindCurrentFrameBuffer();
}

void WaterFrameBuffers::BindFrameBuffer(const uint& frame_buffer, const int& width, const int& height)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glViewport(0, 0, width, height);
}

uint WaterFrameBuffers::CreateFrameBuffer()
{
	uint frame_buffer;
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	
	return frame_buffer;
}

uint WaterFrameBuffers::CreateTextureAttachment(const int &width, const int& height)
{
	uint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		texture, 0);

	return texture;
}

uint WaterFrameBuffers::CreateDepthTextureAttachment(const int& width, const int& height)
{
	uint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		texture, 0);

	return texture;
}

uint WaterFrameBuffers::CreateDepthBufferAttachment(const int& width, const int& height)
{
	uint depth_buffer;
	glGenRenderbuffers(1, &depth_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

	return depth_buffer;
}
