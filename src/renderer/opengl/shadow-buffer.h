#ifndef ACTIO_SHADOW_BUFFER_H
#define ACTIO_SHADOW_BUFFER_H

#include <glad/glad.h>
#include "../renderer_types.h"

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

        private:
            GLuint fbo = 0;
            GLuint shadowMap = 0;
            GLenum status = GL_FALSE;
        };
    }
}

#endif //ACTIO_SHADOW_BUFFER_H
