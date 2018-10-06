#include "shadow-buffer.h"
#include "../../lib/console.h"

namespace renderer {
    namespace Opengl {
        ShadowBuffer::~ShadowBuffer() {
            if (fbo != 0) {
                glDeleteFramebuffers(1, &fbo);
            }
        }

        void ShadowBuffer::bindForWriting() {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        }

        void ShadowBuffer::bindForReading(const GLenum &textureUnit) {
            glActiveTexture(textureUnit);
            glBindTexture(GL_TEXTURE_2D, shadowMap);
        }

        bool ShadowBuffer::init(const GLsizei &width, const GLsizei &height) {
            glGenFramebuffers(1, &fbo);

            glGenTextures(1, &shadowMap);
            glBindTexture(GL_TEXTURE_2D, shadowMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
            glDrawBuffer(GL_NONE);

            status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

            return status == GL_FRAMEBUFFER_COMPLETE;
        }

        bool ShadowBuffer::isCompleted() {
            return status == GL_FRAMEBUFFER_COMPLETE;
        }
    }
}