#include "g-buffer.h"

namespace Renderer {

GBuffer::GBuffer()
{

}

GBuffer::~GBuffer()
{

}

bool GBuffer::init(Renderer::ScreenSize width, Renderer::ScreenSize height)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	unsigned int countTextures = ARRAY_SIZE(textures);
	glGenTextures(countTextures, textures);
	glGenTextures(1, &depthTexture);

	for (unsigned int i = 0; i < countTextures; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
	}

	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	GLenum drawBuffers[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};

	unsigned int countBuffers = ARRAY_SIZE(drawBuffers);

	if (countBuffers > GL_MAX_COLOR_ATTACHMENTS)
	{
		console::warn("count color attachments: ", countBuffers, ", max: ", GL_MAX_COLOR_ATTACHMENTS);
	}

	glDrawBuffers(countBuffers, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		console::err("framebuffer error: ", status);
		return false;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

void GBuffer::bindForWriting() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); }
void GBuffer::bindForReading() { glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo); }

void GBuffer::setReadBuffer(GBUFFER_TEXTURE_TYPE textureType)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + textureType);
}

}
