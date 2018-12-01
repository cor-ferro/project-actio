#ifndef ACTIO_RENDERER_OPENGL_UTILS_H
#define ACTIO_RENDERER_OPENGL_UTILS_H

#include <glad/glad.h>
#include "../../core/texture.h"
#include "../../core/geometry.h"
#include "handle.h"

namespace renderer {
    namespace Opengl {
        namespace utils {
            void bindTexture(const renderer::Opengl::TextureHandle *handle);

            void bindTexture(GLenum targetUnit, const renderer::Opengl::TextureHandle *handle);

            bool checkErrors(const char *file, int line, bool silent = false);

            void fillParam(const GLenum& param, GLboolean& value);

            void fillParam(const GLenum& param, GLdouble& value);

            void fillParam(const GLenum& param, GLfloat& value);

            void fillParam(const GLenum& param, GLint& value);

            void fillParam(const GLenum& param, GLint64& value);

            template<typename T>
            const T getParam(const GLenum& param) {
                T value;
                fillParam(param, value);

                return value;
            }
        }
    }
}

#define OpenglCheckErrors() renderer::Opengl::utils::checkErrors(__FILE__, __LINE__)
#define OpenglCheckErrorsSilent() renderer::Opengl::utils::checkErrors(__FILE__, __LINE__, true)

#endif //ACTIO_RENDERER_OPENGL_UTILS_H
