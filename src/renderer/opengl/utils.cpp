#include "utils.h"

bool OpenglUtils::checkErrors(const char *file, int line, bool silent) {
    GLenum err = glGetError();

    if (err == GL_NO_ERROR) return false;
    if (silent) return true;

    std::string error;

    switch (err) {
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            error = "unknown gl error";
    }

    if (!error.empty()) {
        console::err("GL: %s [%s:%i]", error, file, line);
    }

    return true;
}

void OpenglUtils::bindTexture(const renderer::Opengl::TextureHandle *handle) {
    glBindTexture(handle->target, handle->textureId);
}

void OpenglUtils::bindTexture(GLenum targetUnit, const renderer::Opengl::TextureHandle *handle) {
    glActiveTexture(targetUnit);
    bindTexture(handle);
}

