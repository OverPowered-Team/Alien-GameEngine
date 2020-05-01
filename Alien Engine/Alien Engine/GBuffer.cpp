#include "GBuffer.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "Viewport.h"

GBuffer::GBuffer()
{
	GenerateGBuffer();

	for (uint i = 0; i < 3; ++i)
	{
		attachments[i] = 0;
	}
}

GBuffer::~GBuffer()
{
	DestroyGBuffer();

	for (uint i = 0; i < 3; ++i)
	{
		attachments[i] = 0;
	}
}

void GBuffer::GenerateGBuffer()
{
	glGenFramebuffers(1, &gBuffer);

	// Texture buffers
	glGenTextures(1, &gPosition);
	glGenTextures(1, &gNormals);
	glGenTextures(1, &gColorSpec);
}

void GBuffer::DestroyGBuffer()
{
	glDeleteFramebuffers(1, &gBuffer);

	// Texture buffers
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormals);
	glDeleteTextures(1, &gColorSpec);
}

void GBuffer::BeginGBuffer()
{
	if (to_update)
	{
		UpdateTextures();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::EndGBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::UpdateTextures()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// Update Position Texture Buffer 

	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// Update Normals Texture Buffer 

	glBindTexture(GL_TEXTURE_2D, gNormals);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormals, 0);

	// Update color + specular color buffer
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// Unbind Everything
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void GBuffer::SetSize(float width, float height)
{
	this->width = width;
	this->height = height;
	to_update = true;
}
