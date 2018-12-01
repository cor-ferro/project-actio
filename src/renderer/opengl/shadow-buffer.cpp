#include "shadow-buffer.h"
#include "../../lib/console.h"
#include "utils.h"

namespace renderer {
    namespace Opengl {
        ShadowBuffer::~ShadowBuffer() {
            if (fbo != 0) {
                glDeleteFramebuffers(1, &fbo);
            }

            destroyShadowDepthTexture();
            destroyShadowMapTexture();
        }

        void ShadowBuffer::bindForWriting() {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        }

        void ShadowBuffer::bindForReading(const GLenum &textureUnit) {
            glActiveTexture(textureUnit);
            glBindTexture(GL_TEXTURE_2D, shadowDepth);
        }

        bool ShadowBuffer::init(const GLsizei &width, const GLsizei &height) {
            this->width = width;
            this->height = height;

            glGenFramebuffers(1, &fbo);
            OpenglCheckErrors();

            generateShadowDepthTexture();
            OpenglCheckErrors();
            generateShadowMapTexture();
            OpenglCheckErrors();

//            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
//            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepth, 0);
//            glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);

            OpenglCheckErrors();

            glDrawBuffer(GL_NONE);
            OpenglCheckErrors();
            glReadBuffer(GL_NONE);
            OpenglCheckErrors();

            status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

            OpenglCheckErrors();

            if (status != GL_FRAMEBUFFER_COMPLETE) {
                const char * message = "";
                switch (status) {
                    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: message = "INCOMPLETE_ATTACHMENT"; break;
                    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: message = "INCOMPLETE_MISSING_ATTACHMENT"; break;
                    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: message = "INCOMPLETE_DRAW_BUFFER"; break;
                    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: message = "INCOMPLETE_READ_BUFFER"; break;
                    case GL_FRAMEBUFFER_UNSUPPORTED: message = "UNSUPPORTED"; break;
                    default:
                        message = "Unknown frame buffer error";
                }

                console::err("framebuffer error: %s", message);
            }

            return status == GL_FRAMEBUFFER_COMPLETE;
        }

        bool ShadowBuffer::isCompleted() {
            return status == GL_FRAMEBUFFER_COMPLETE;
        }

        void ShadowBuffer::generateShadowDepthTexture() {
            destroyShadowDepthTexture();

            // depth buffer
            glGenTextures(1, &shadowDepth);
            glBindTexture(GL_TEXTURE_2D, shadowDepth);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            OpenglCheckErrors();
        }

        void ShadowBuffer::generateShadowMapTexture() {
            destroyShadowMapTexture();

            glGenTextures(1, &shadowMap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);

            for (uint i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                OpenglCheckErrors();
            }

            OpenglCheckErrors();
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            OpenglCheckErrors();
        }

        void ShadowBuffer::destroyShadowDepthTexture() {
            if (shadowDepth != 0) {
                glDeleteTextures(1, &shadowDepth);
                shadowDepth = 0;
            }

        }

        void ShadowBuffer::destroyShadowMapTexture() {
            if (shadowMap != 0) {
                glDeleteTextures(1, &shadowMap);
                shadowMap = 0;
            }
        }

        GLuint ShadowBuffer::getShadowMap() {
            return shadowMap;
        }
    }
}