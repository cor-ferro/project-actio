#include "utils.h"

namespace OpenglUtils {
    GLenum textureTarget(const Texture &texture) {
        GLenum target;

        switch (texture.type) {
            case Texture::Type::Diffuse:
            case Texture::Type::Specular:
            case Texture::Type::Height:
            case Texture::Type::Normal:
                target = GL_TEXTURE_2D;
                break;
            case Texture::Type::Cube:
                target = GL_TEXTURE_CUBE_MAP;
                break;
            default:
                target = GL_TEXTURE_2D;
        }

        return target;
    }

    void bindTexture(const renderer::Opengl::TextureHandle *handle) {
        glBindTexture(handle->target, handle->textureId);
    }

    void bindTexture(GLenum targetUnit, const renderer::Opengl::TextureHandle *handle) {
        glActiveTexture(targetUnit);
        bindTexture(handle);
    }

    void checkErrors(const char *file, int line, bool silent) {
        GLenum err = glGetError();

        if (silent == true) return;
        if (err == GL_NO_ERROR) return;

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

        if (error.size() != 0) {
            console::err("GL: %s [%s:%i]", error, file, line);
        }
    }
}
