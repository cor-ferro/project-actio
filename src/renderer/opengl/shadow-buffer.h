#ifndef ACTIO_SHADOW_BUFFER_H
#define ACTIO_SHADOW_BUFFER_H

#include <glad/glad.h>
#include "../params.h"

namespace renderer {
    namespace Opengl {
        class ShadowBuffer {
        public:
            ShadowBuffer() = default;
            ~ShadowBuffer();

            bool init(const GLsizei &width, const GLsizei &height);

            void bindForWriting();
            void bindForReading(const GLenum &textureUnit);

            bool isCompleted();

            GLuint getShadowMap();

        private:
            void generateShadowDepthTexture();
            void generateShadowMapTexture();

            void destroyShadowDepthTexture();
            void destroyShadowMapTexture();

            GLuint fbo = 0;
            GLuint shadowDepth = 0;
            GLuint shadowMap = 0;
            GLenum status = GL_FALSE;

            GLsizei width = 0;
            GLsizei height = 0;
        };
    }
}

#endif //ACTIO_SHADOW_BUFFER_H
