#include "g-buffer.h"

namespace renderer {
	namespace Opengl {
		GBuffer::GBuffer()
		{

		}

		GBuffer::~GBuffer()
		{
			if (fbo != 0) {
				glDeleteFramebuffers(1, &fbo);
			}

			if (textures.size() != 0) {
				glDeleteTextures(textures.size(), &textures.front());
				textures.clear();
			}

			if (rboDepth != 0) {
				glDeleteRenderbuffers(1, &rboDepth);
			}

			if (depthTexture != 0) {
				glDeleteTextures(1, &depthTexture);
			}
		}

		bool GBuffer::init(renderer::ScreenSize width, renderer::ScreenSize height)
		{
			console::info("gbuffer init: %i, %i", width, height);
			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			{
				GLuint texId;
				glGenTextures(1, &texId);
				textures.push_back(texId);

				glBindTexture(GL_TEXTURE_2D, texId);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
			}

			{
				GLuint texId;
				glGenTextures(1, &texId);
				textures.push_back(texId);

				glBindTexture(GL_TEXTURE_2D, texId);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texId, 0);
			}

			{
				GLuint texId;
				glGenTextures(1, &texId);
				textures.push_back(texId);

				glBindTexture(GL_TEXTURE_2D, texId);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texId, 0);
			}

			glGenTextures(1, &depthTexture);
			glBindTexture(GL_TEXTURE_2D, depthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

			glGenTextures(1, &finalTexture);
			glBindTexture(GL_TEXTURE_2D, finalTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, finalTexture, 0);


			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			console::info("framebuffer status: %i", status);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				const char * message = "";
				switch (status) {
					case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: message = "INCOMPLETE_ATTACHMENT"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: message = "INCOMPLETE_MISSING_ATTACHMENT"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: message = "INCOMPLETE_DRAW_BUFFER"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: message = "INCOMPLETE_READ_BUFFER"; break;
					case GL_FRAMEBUFFER_UNSUPPORTED: message = "UNSUPPORTED"; break;
				}

				console::err("framebuffer error: %s", message);

				return false;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return true;
		}

		void GBuffer::startFrame()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
			glDrawBuffer(GL_COLOR_ATTACHMENT4);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		void GBuffer::geometryPass()
		{
			const uint drawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
			glDrawBuffers(3, drawBuffers);
		}

		void GBuffer::lightPass()
		{
			const uint drawBuffers[1] = { GL_COLOR_ATTACHMENT4 };

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
			glDrawBuffers(1, drawBuffers);
		}

		void GBuffer::stencilPass()
		{
			glDrawBuffer(GL_NONE);
		}

		void GBuffer::finalPass()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT4);
		}

		void GBuffer::bindForWriting() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); }
		void GBuffer::bindForReading() { glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo); }

		void GBuffer::setReadBuffer(GBUFFER_TEXTURE_TYPE textureType)
		{
			glReadBuffer(GL_COLOR_ATTACHMENT0 + textureType);
		}

		const std::vector<GLuint>& GBuffer::getTextures()
		{
			return textures;
		}
	}
}
