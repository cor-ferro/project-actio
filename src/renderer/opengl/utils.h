//
// Created by demitriy on 10/6/18.
//

#ifndef ACTIO_RENDERER_OPENGL_UTILS_H
#define ACTIO_RENDERER_OPENGL_UTILS_H

#include <glad/glad.h>
#include "../../core/texture.h"
#include "../../core/geometry.h"
#include "handle.h"

namespace OpenglUtils {
    void bindTexture(const renderer::Opengl::TextureHandle *handle);
    void bindTexture(GLenum targetUnit, const renderer::Opengl::TextureHandle *handle);
    bool checkErrors(const char * file, int line, bool silent = false);
}

#define OpenglCheckErrors() OpenglUtils::checkErrors(__FILE__, __LINE__)
#define OpenglCheckErrorsSilent() OpenglUtils::checkErrors(__FILE__, __LINE__, true)

#endif //ACTIO_RENDERER_OPENGL_UTILS_H
